#include <assert.h>
#include "cpu_whvp.h"

#include "openxbox/log.h"

namespace openxbox {

WhvpCpu::WhvpCpu() {
	m_whvp = nullptr;
	m_partition = nullptr;
	m_vcpu = nullptr;

    m_pendingInterruptsBitmap = 0;
}

WhvpCpu::~WhvpCpu() {
	if (m_whvp != nullptr) {
		// Deleting this object will automatically delete the partition and VCPU
		delete m_whvp;
		m_whvp = nullptr;
	}

	// Clear the physical memory map
	for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
		delete *it;
	}
	m_physMemMap.clear();
}

int WhvpCpu::InitializeImpl() {
	if (m_whvp == nullptr) {
        m_whvp = new WinHvPlatform();

		if (!m_whvp->IsPresent()) {
			return WHVS_FAILED;
		}

		auto partStatus = m_whvp->CreatePartition(&m_partition);
		if (partStatus != WHVPS_SUCCESS) {
			delete m_whvp;
			m_whvp = nullptr;
			return partStatus;
		}

        // Give one processor to the partition
        WHV_PARTITION_PROPERTY partitionProperty;
        partitionProperty.ProcessorCount = 1;
        partStatus = m_partition->SetProperty(WHvPartitionPropertyCodeProcessorCount, &partitionProperty);
        if (WHVPS_SUCCESS != partStatus) {
            return partStatus;
        }

        // Setup the partition
        partStatus = m_partition->Setup();
        if (WHVPS_SUCCESS != partStatus) {
            return partStatus;
        }

		auto vcpuStatus = m_partition->CreateVCPU(&m_vcpu, 0);
		if (vcpuStatus != WHVVCPUS_SUCCESS) {
			delete m_whvp;
			m_whvp = nullptr;
			return vcpuStatus;
		}

        m_vcpu->SetIoPortCallback(IoPortCallback);
        m_vcpu->SetMemoryCallback(MemoryCallback);

        // Windows Hypervisor Platform VCPUs are initialized to EIP = 0xFFFF0,
        // but we expect it to be 0xFFFFFFF0 instead, so let's set this here
        RegWrite(REG_EIP, 0xFFFFFFF0);

        m_interruptHandlerCredits = kInterruptHandlerMaxCredits;
    }

	return 0;
}

int WhvpCpu::RunImpl() {
    // Increment the credits available for the interrupt handler
    if (m_interruptHandlerCredits < kInterruptHandlerMaxCredits) {
        m_interruptHandlerCredits += kInterruptHandlerIncrement;
    }

    // Inject an interrupt if available and possible
    if (m_pendingInterruptsBitmap != 0) {
        InjectPendingInterrupt();
    }

	// Run CPU
	auto status = m_vcpu->Run();

	// Check VM exit status
	if (status != WHVVCPUS_SUCCESS) {
		return -1;
	}

	// Handle exit status using tunnel
	switch (m_vcpu->ExitContext()->ExitReason) {
    case WHvRunVpExitReasonX64Halt:                  m_exitInfo.reason = CPU_EXIT_HLT;       break;  // HLT instruction
    case WHvRunVpExitReasonX64IoPortAccess:          m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // I/O (in / out instructions)
    case WHvRunVpExitReasonMemoryAccess:             m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // MMIO
	case WHvRunVpExitReasonX64InterruptWindow:       m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Interrupt window (never requested, should never happen)
    case WHvRunVpExitReasonCanceled:                 m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Execution cancelled (should never happen because OpenXBOX does not use this operation)
    case WHvRunVpExitReasonNone:                     m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // VM exited for no reason
    case WHvRunVpExitReasonException:                m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // VCPU threw an unhandled exception (the kernel should handle them)
    case WHvRunVpExitReasonX64Cpuid:                 m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // CPUID instruction (not enabled, should never happen)
    case WHvRunVpExitReasonX64MsrAccess:             m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // MSR access (not enabled, should never happen)
	case WHvRunVpExitReasonUnsupportedFeature:       m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Host CPU does not support a feature needed by the VM
	case WHvRunVpExitReasonInvalidVpRegisterValue:   m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // VCPU has an invalid register
    case WHvRunVpExitReasonUnrecoverableException:   m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Unrecoverable exception
	}

	return 0;
}

int WhvpCpu::StepImpl(uint64_t num_instructions) {
    // FIXME: would be nice if we didn't have to interfere with the VCPU state
	for (uint64_t i = 0; i < num_instructions; i++) {
		// Set the trap flag. This will be cleared on the next instruction
		SetFlags(TF_MASK);

		// Run CPU
		int result = RunImpl();

		// Exit on failure or any other exit reason
		if (result) {
			return result;
		}
		if (m_exitInfo.reason != CPU_EXIT_NORMAL) {
			return 0;
		}
	}

	return 0;
}

InterruptResult WhvpCpu::InterruptImpl(uint8_t vector) {
    // Acquire the pending interrupts mutex
    std::lock_guard<std::mutex> guard(m_pendingInterruptsMutex);

    // If the vector has not been enqueued yet, enqueue the interrupt
    if (!Bitmap64IsSet(m_pendingInterruptsBitmap, vector)) {
        m_pendingInterrupts.push(vector);
        Bitmap64Set(&m_pendingInterruptsBitmap, vector);
    }
    // Keep track of skipped interrupts
    else {
        m_skippedInterrupts[vector]++;
    }

    // Cancel execution of the VCPU
    m_vcpu->CancelRun();

	return INTR_SUCCESS;
}

int WhvpCpu::MemMapSubregion(MemoryRegion *subregion) {
	switch (subregion->m_type) {
	case MEM_REGION_MMIO: {
		// TODO: map MMIO range
		// When MMIO happens, handle similar to xb_uc_hook
		// subregion->m_handler contains the handler function pointer
		// subregion->m_handler_user contains user data for the handler

		// All unmapped regions in a HAXM VM are MMIO, no need to allocate
		return 0;
	}
	case MEM_REGION_NONE: {
		// This should not happen
		assert(0);
		return -1;
	}
	case MEM_REGION_RAM:
	case MEM_REGION_ROM: {
		// Region is either RAM or ROM, map it accordingly
		WHV_MAP_GPA_RANGE_FLAGS flags = WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagExecute | ((subregion->m_type == MEM_REGION_RAM) ? WHvMapGpaRangeFlagWrite : WHvMapGpaRangeFlagNone);
		auto status = m_partition->MapGpaRange(subregion->m_data, subregion->m_start, subregion->m_size, flags);
		if (status) { return status; }
		
		// Map the physical address range
		m_physMemMap.push_back(new PhysicalMemoryRange{ (char *)subregion->m_data, subregion->m_start, subregion->m_start + (uint32_t)subregion->m_size - 1 });

		return 0;
	}
	}

	return -1;
}

int WhvpCpu::MemRead(uint32_t addr, uint32_t size, void *value) {
	for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
		auto physMemRegion = *it;
		if (addr >= physMemRegion->startingAddress && addr <= physMemRegion->endingAddress) {
			memcpy(value, &physMemRegion->data[addr - physMemRegion->startingAddress], size);
			return 0;
		}
	}

	return -1;
}

int WhvpCpu::MemWrite(uint32_t addr, uint32_t size, void *value) {
	for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
		auto physMemRegion = *it;
		if (addr >= physMemRegion->startingAddress && addr <= physMemRegion->endingAddress) {
			memcpy(&physMemRegion->data[addr - physMemRegion->startingAddress], value, size);
			return 0;
		}
	}

	return -1;
}

int WhvpCpu::RegRead(enum CpuReg reg, uint32_t *value) {
    WHV_REGISTER_NAME regs[1];
    WHV_REGISTER_VALUE val[1];

	switch (reg) {
    case REG_EIP:    regs[0] = WHvX64RegisterRip;    break;
    case REG_EFLAGS: regs[0] = WHvX64RegisterRflags; break;
    case REG_EAX:    regs[0] = WHvX64RegisterRax;    break;
	case REG_ECX:    regs[0] = WHvX64RegisterRcx;    break;
	case REG_EDX:    regs[0] = WHvX64RegisterRdx;    break;
	case REG_EBX:    regs[0] = WHvX64RegisterRbx;    break;
    case REG_ESI:    regs[0] = WHvX64RegisterRsi;    break;
    case REG_EDI:    regs[0] = WHvX64RegisterRdi;    break;
    case REG_ESP:    regs[0] = WHvX64RegisterRsp;    break;
    case REG_EBP:    regs[0] = WHvX64RegisterRbp;    break;
	case REG_CS:     regs[0] = WHvX64RegisterCs;     break;
	case REG_SS:     regs[0] = WHvX64RegisterSs;     break;
	case REG_DS:     regs[0] = WHvX64RegisterDs;     break;
	case REG_ES:     regs[0] = WHvX64RegisterEs;     break;
	case REG_FS:     regs[0] = WHvX64RegisterFs;     break;
	case REG_GS:     regs[0] = WHvX64RegisterGs;     break;
    case REG_TR:     regs[0] = WHvX64RegisterTr;     break;
    case REG_CR0:    regs[0] = WHvX64RegisterCr0;    break;
    case REG_CR2:    regs[0] = WHvX64RegisterCr2;    break;
    case REG_CR3:    regs[0] = WHvX64RegisterCr3;    break;
    case REG_CR4:    regs[0] = WHvX64RegisterCr4;    break;
	default:                                         return -1;
	}

    auto status = m_vcpu->GetRegisters(regs, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return status;
    }

    switch (reg) {
    case REG_EIP: case REG_EFLAGS: case REG_EAX: case REG_ECX: case REG_EDX: case REG_EBX:
    case REG_ESI: case REG_EDI: case REG_ESP: case REG_EBP: case REG_CR0: case REG_CR2: case REG_CR3: case REG_CR4:
        *value = val[0].Reg32;
        break;
    case REG_CS: case REG_SS: case REG_DS: case REG_ES: case REG_FS: case REG_GS: case REG_TR:
        *value = val[0].Segment.Selector;
        break;
    default:
        return -1;
    }

	return 0;
}

int WhvpCpu::RegWrite(enum CpuReg reg, uint32_t value) {
    WHV_REGISTER_NAME regs[1];
    WHV_REGISTER_VALUE val[1];

    switch (reg) {
    case REG_EIP:    regs[0] = WHvX64RegisterRip;    break;
    case REG_EFLAGS: regs[0] = WHvX64RegisterRflags; break;
    case REG_EAX:    regs[0] = WHvX64RegisterRax;    break;
    case REG_ECX:    regs[0] = WHvX64RegisterRcx;    break;
    case REG_EDX:    regs[0] = WHvX64RegisterRdx;    break;
    case REG_EBX:    regs[0] = WHvX64RegisterRbx;    break;
    case REG_ESI:    regs[0] = WHvX64RegisterRsi;    break;
    case REG_EDI:    regs[0] = WHvX64RegisterRdi;    break;
    case REG_ESP:    regs[0] = WHvX64RegisterRsp;    break;
    case REG_EBP:    regs[0] = WHvX64RegisterRbp;    break;
    case REG_CS:     regs[0] = WHvX64RegisterCs;     break;
    case REG_SS:     regs[0] = WHvX64RegisterSs;     break;
    case REG_DS:     regs[0] = WHvX64RegisterDs;     break;
    case REG_ES:     regs[0] = WHvX64RegisterEs;     break;
    case REG_FS:     regs[0] = WHvX64RegisterFs;     break;
    case REG_GS:     regs[0] = WHvX64RegisterGs;     break;
    case REG_TR:     regs[0] = WHvX64RegisterTr;     break;
    case REG_CR0:    regs[0] = WHvX64RegisterCr0;    break;
    case REG_CR2:    regs[0] = WHvX64RegisterCr2;    break;
    case REG_CR3:    regs[0] = WHvX64RegisterCr3;    break;
    case REG_CR4:    regs[0] = WHvX64RegisterCr4;    break;
    default:                                         return -1;
    }

    switch (reg) {
    case REG_EIP: case REG_EFLAGS: case REG_EAX: case REG_ECX: case REG_EDX: case REG_EBX:
    case REG_ESI: case REG_EDI: case REG_ESP: case REG_EBP: case REG_CR0: case REG_CR2: case REG_CR3: case REG_CR4:
        val[0].Reg32 = value;
        break;
    case REG_CS: case REG_SS: case REG_DS: case REG_ES: case REG_FS: case REG_GS: case REG_TR:
    {
        val[0].Segment.Selector = value;

        // Get GDT entry from memory
        uint32_t gdtAddr;
        uint32_t gdtSize;
        GetGDT(&gdtAddr, &gdtSize);

        GDTEntry gdtEntry;
        VMemRead(gdtAddr + value, sizeof(GDTEntry), &gdtEntry);

        // Fill in the rest of the CS info with data from the GDT entry
        val[0].Segment.Attributes = gdtEntry.data.access | (gdtEntry.data.flags << 12);
        val[0].Segment.Base = gdtEntry.GetBase();
        val[0].Segment.Limit = gdtEntry.GetLimit();
        break;
    }
    default:
        return -1;
    }

    auto status = m_vcpu->SetRegisters(regs, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return status;
    }

    return 0;
}

int WhvpCpu::GetGDT(uint32_t *addr, uint32_t *size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterGdtr };
    WHV_REGISTER_VALUE val[1];
    auto status = m_vcpu->GetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return status;
    }
	*addr = val[0].Table.Base;
	*size = val[0].Table.Limit;

	return 0;
}

int WhvpCpu::SetGDT(uint32_t addr, uint32_t size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterGdtr };
    WHV_REGISTER_VALUE val[1];
    val[0].Table.Base = addr;
    val[0].Table.Limit = size;

    auto status = m_vcpu->SetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return status;
    }

    return 0;
}

int WhvpCpu::GetIDT(uint32_t *addr, uint32_t *size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterIdtr };
    WHV_REGISTER_VALUE val[1];
    auto status = m_vcpu->GetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return status;
    }
    *addr = val[0].Table.Base;
    *size = val[0].Table.Limit;

    return 0;
}

int WhvpCpu::SetIDT(uint32_t addr, uint32_t size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterIdtr };
    WHV_REGISTER_VALUE val[1];
    val[0].Table.Base = addr;
    val[0].Table.Limit = size;

    auto status = m_vcpu->SetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return status;
    }

    return 0;
}

int WhvpCpu::InjectInterrupt(uint8_t vector) {
    return m_vcpu->Interrupt(vector);
}

HRESULT WhvpCpu::IoPortCallback(PVOID context, WHV_EMULATOR_IO_ACCESS_INFO *io) {
    WhvpCpu *cpu = (WhvpCpu *) context;
    if (io->Direction == WHV_IO_OUT) {
        cpu->m_ioMapper->IOWrite(io->Port, io->Data, io->AccessSize);
    }
    else {
        cpu->m_ioMapper->IORead(io->Port, &io->Data, io->AccessSize);
    }
    return S_OK;
}

HRESULT WhvpCpu::MemoryCallback(PVOID context, WHV_EMULATOR_MEMORY_ACCESS_INFO *mem) {
    WhvpCpu *cpu = (WhvpCpu *)context;
    if (mem->Direction == WHV_IO_OUT) {
        cpu->m_ioMapper->MMIOWrite(mem->GpaAddress, *(uint32_t *)mem->Data, mem->AccessSize);
    }
    else {
        cpu->m_ioMapper->MMIORead(mem->GpaAddress, (uint32_t *)mem->Data, mem->AccessSize);
    }
    return S_OK;
}

}
