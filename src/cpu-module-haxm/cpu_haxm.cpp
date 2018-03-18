#include <assert.h>
#include "cpu_haxm.h"

#include "openxbox/log.h"

namespace openxbox {

static bool parity8(uint8_t x) {
	uint8_t y = x ^ (x >> 1);
	y = y ^ (y >> 2);
	y = y ^ (y >> 4);
	
	return (y & 1) == 0;
}

HaxmCpu::HaxmCpu() {
	m_haxm = nullptr;
	m_vm = nullptr;
	m_vcpu = nullptr;

	m_regsDirty = true;
	m_fpuRegsDirty = true;
	m_regsChanged = false;
	m_fpuRegsChanged = false;

    m_pendingInterruptsBitmap = 0;
}

HaxmCpu::~HaxmCpu() {
	if (m_haxm != nullptr) {
		// Deleting this object will automatically delete the VM and VCPU
		delete m_haxm;
		m_haxm = nullptr;
	}

	// Clear the physical memory map
	for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
		delete *it;
	}
	m_physMemMap.clear();
}

int HaxmCpu::InitializeImpl() {
	if (m_haxm == nullptr) {
        m_haxm = new Haxm();

		auto status = m_haxm->Initialize();
		if (status != HXS_SUCCESS) {
			return status;
		}

		auto vmStatus = m_haxm->CreateVM(&m_vm);
		if (vmStatus != HXVMS_SUCCESS) {
			delete m_haxm;
			m_haxm = nullptr;
			return vmStatus;
		}

		auto vcpuStatus = m_vm->CreateVCPU(&m_vcpu);
		if (vcpuStatus != HXVCPUS_SUCCESS) {
			delete m_haxm;
			m_haxm = nullptr;
			return vcpuStatus;
		}

        m_interruptHandlerCredits = kInterruptHandlerMaxCredits;
    }

	return 0;
}

int HaxmCpu::RunImpl() {
	// Update CPU state if registers were modified
	if (m_regsChanged) {
		m_vcpu->SetRegisters(&m_regs);
		m_regsChanged = false;
	}
	if (m_fpuRegsChanged) {
		m_vcpu->SetFPURegisters(&m_fpuRegs);
		m_fpuRegsChanged = false;
	}

    auto tunnel = m_vcpu->Tunnel();
 
    // Increment the credits available for the interrupt handler
    if (m_interruptHandlerCredits < kInterruptHandlerMaxCredits) {
        m_interruptHandlerCredits += kInterruptHandlerIncrement;
    }

    // Inject an interrupt if available and possible
    if (m_pendingInterruptsBitmap != 0) {
        if (tunnel->ready_for_interrupt_injection) {
            InjectPendingInterrupt();
        }
        // Request an interrupt window if the VCPU is not ready
        else {
            tunnel->request_interrupt_window = 1;
        }
    }

	// Run CPU
	auto status = m_vcpu->Run();

	// Mark registers as dirty
	m_regsDirty = true;
	m_fpuRegsDirty = true;

	// Check VM exit status
	if (status == HXVCPUS_FAILED) {
		return -1;
	}

	// Handle exit status using tunnel
	switch (tunnel->_exit_status) {
    case HAX_EXIT_HLT:         m_exitInfo.reason = CPU_EXIT_HLT;       break;  // HLT instruction
	case HAX_EXIT_IO:          m_exitInfo.reason = CPU_EXIT_NORMAL;            // I/O (in / out instructions)
		return HandleIO(tunnel->io._df, tunnel->io._port, tunnel->io._direction, tunnel->io._size, tunnel->io._count, m_vcpu->IOTunnel());
	case HAX_EXIT_FAST_MMIO:   m_exitInfo.reason = CPU_EXIT_NORMAL;            // Fast MMIO
        return HandleFastMMIO((struct hax_fastmmio *)m_vcpu->IOTunnel());
	case HAX_EXIT_INTERRUPT:   m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Let HAXM handle this
	case HAX_EXIT_PAUSED:      m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Let HAXM handle this
    case HAX_EXIT_MMIO:        m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Regular MMIO (cannot be implemented)
	case HAX_EXIT_REALMODE:    m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Real mode is not supported
	case HAX_EXIT_UNKNOWN:     m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // VM failed for an unknown reason
	case HAX_EXIT_STATECHANGE: m_exitInfo.reason = CPU_EXIT_SHUTDOWN;  break;  // The VM is shutting down
	}

	// HACK!
	// HAXM 7.0.0 has limited support for MMIO instructions. Early during the
	// bootloader stage, the VCPU stops at the following instruction:
	//   test dword ptr [ebx + 0x101000], 0xc0000
	//   (ebx = 0xfd000000)
	// The instruction bytes are:
	//   f7 83 00 10 10 00 00 00 0c 00
	// So if the VCPU "dies" with HAX_EXIT_STATECHANGE, we check for those
	// bytes at EIP and if they match, we emulate the instruction, skip it and
	// recreate the VCPU.
	if (tunnel->_exit_status == HAX_EXIT_STATECHANGE) {
		// Check next 10 bytes at EIP
		uint32_t eip;
		RegRead(REG_EIP, &eip);
		uint32_t instr[3];
		memset(instr, 0, sizeof(instr));
		VMemRead(eip, 10, instr);
		if (instr[0] == 0x100083f7 && instr[1] == 0x00000010 && (instr[2] & 0xffff) == 0x000c) {
			log_debug("HaxmCpu: Emulating MMIO instruction: test dword ptr [ebx + 0x101000], 0xc0000\n");
			
			// Read the MMIO address
			uint32_t ebx;
			RegRead(REG_EBX, &ebx);

			uint32_t tmp;
			m_ioMapper->MMIORead(ebx + 0x101000, &tmp, sizeof(uint32_t));

			// Compute AND result
			uint32_t result = tmp & 0xc0000;
			
			// Update flags
			// TEST updates flags as follows:
			// - CF and OF are set to 0
			// - SF is set to the highest bit of the result
			// - ZF is set to 1 if result is zero, 0 otherwise
			// - PF is set to 1 if the number of bits in the least significant byte is even, 0 if odd

			uint32_t bitsToClear = CF_MASK | OF_MASK;
			uint32_t bitsToSet = 0;
			if (result & 0x80000000) { bitsToSet |= SF_MASK; } else { bitsToClear |= SF_MASK; }
			if (result == 0) { bitsToSet |= ZF_MASK; } else { bitsToClear |= ZF_MASK; }
			if (parity8(result & 0xff)) { bitsToSet |= PF_MASK; } else { bitsToClear |= PF_MASK; }

			uint32_t eflags;
			RegRead(REG_EFLAGS, &eflags);
			eflags = eflags & ~(bitsToClear) | bitsToSet;
			RegWrite(REG_EFLAGS, eflags);

			// Go to next instruction
			eip += 10;
			RegWrite(REG_EIP, eip);
			
			// Tell the emulator we ran normally
			m_exitInfo.reason = CPU_EXIT_NORMAL;

			// Copy full VCPU state
			vcpu_state_t state;
			fx_layout fpu;
			hax_msr_data msr;
			memset(&state, 0, sizeof(state));
			memset(&fpu, 0, sizeof(fpu));
			memset(&msr, 0, sizeof(msr));

			// TODO: there are more MSRs, but these are the only ones known to
			// be modified by the Xbox kernel
			uint8_t mi = 0;
			msr.entries[mi++].entry = 0x00000277;
			msr.entries[mi++].entry = 0x000000fe;
			msr.entries[mi++].entry = 0x000002ff;
			msr.entries[mi++].entry = 0x00000250;
			msr.entries[mi++].entry = 0x00000258;
			msr.entries[mi++].entry = 0x00000259;
			msr.entries[mi++].entry = 0x00000268;
			msr.entries[mi++].entry = 0x00000269;
			
			msr.entries[mi++].entry = 0x0000026a;
			msr.entries[mi++].entry = 0x0000026b;
			msr.entries[mi++].entry = 0x0000026c;
			msr.entries[mi++].entry = 0x0000026d;
			msr.entries[mi++].entry = 0x0000026e;
			msr.entries[mi++].entry = 0x0000026f;
			msr.entries[mi++].entry = 0x00000200;
			msr.entries[mi++].entry = 0x00000201;
			
			msr.entries[mi++].entry = 0x00000202;
			msr.entries[mi++].entry = 0x00000203;
			msr.entries[mi++].entry = 0x00000204;
			msr.entries[mi++].entry = 0x00000205;
			msr.entries[mi++].entry = 0x00000206;
			msr.entries[mi++].entry = 0x00000207;
			msr.entries[mi++].entry = 0x00000208;
			msr.entries[mi++].entry = 0x00000209;
			
			msr.entries[mi++].entry = 0x0000020a;
			msr.entries[mi++].entry = 0x0000020b;
			msr.entries[mi++].entry = 0x0000020c;
			msr.entries[mi++].entry = 0x0000020d;
			msr.entries[mi++].entry = 0x0000020e;
			msr.entries[mi++].entry = 0x0000020f;
			msr.entries[mi++].entry = 0x0000001b;
			msr.nr_msr = mi;

			m_vcpu->GetRegisters(&state);
			m_vcpu->GetFPURegisters(&fpu);
			m_vcpu->GetMSRs(&msr);

			// Create a new VCPU, replacing the dead one
			if (m_vm->FreeVCPU(&m_vcpu) != HXVCPUS_SUCCESS) {
				return 1;
			}
			if (m_vm->CreateVCPU(&m_vcpu) != HXVCPUS_SUCCESS) {
				return 1;
			}
			
			// Restore state to the new VCPU
			m_vcpu->SetRegisters(&state);
			m_vcpu->SetFPURegisters(&fpu);
			m_vcpu->SetMSRs(&msr);
		}
	}

	return 0;
}

int HaxmCpu::StepImpl(uint64_t num_instructions) {
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

InterruptResult HaxmCpu::InterruptImpl(uint8_t vector) {
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

	return INTR_SUCCESS;
}

int HaxmCpu::MemMapSubregion(MemoryRegion *subregion) {
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
		HaxmVMMemoryType memType = subregion->m_type == MEM_REGION_RAM ? HXVM_MEM_RAM : HXVM_MEM_ROM;
		auto status = m_vm->AllocateMemory(subregion->m_data, subregion->m_size, subregion->m_start, memType);
		if (status) { return status; }
		
		// Map the physical address range
		m_physMemMap.push_back(new PhysicalMemoryRange{ (char *)subregion->m_data, subregion->m_start, subregion->m_start + (uint32_t)subregion->m_size - 1 });

		return 0;
	}
	}

	return -1;
}

int HaxmCpu::MemRead(uint32_t addr, uint32_t size, void *value) {
	for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
		auto physMemRegion = *it;
		if (addr >= physMemRegion->startingAddress && addr <= physMemRegion->endingAddress) {
			memcpy(value, &physMemRegion->data[addr - physMemRegion->startingAddress], size);
			return 0;
		}
	}

	return -1;
}

int HaxmCpu::MemWrite(uint32_t addr, uint32_t size, void *value) {
	for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
		auto physMemRegion = *it;
		if (addr >= physMemRegion->startingAddress && addr <= physMemRegion->endingAddress) {
			memcpy(&physMemRegion->data[addr - physMemRegion->startingAddress], value, size);
			return 0;
		}
	}

	return -1;
}

int HaxmCpu::RegRead(enum CpuReg reg, uint32_t *value) {
	int status = RefreshRegisters(false);
	if (status) { return status; }

	switch (reg) {
	case REG_EIP:    *value = m_regs._eip;           break;
	case REG_EFLAGS: *value = m_regs._eflags;        break;
	case REG_EAX:    *value = m_regs._eax;           break;
	case REG_ECX:    *value = m_regs._ecx;           break;
	case REG_EDX:    *value = m_regs._edx;           break;
	case REG_EBX:    *value = m_regs._ebx;           break;
	case REG_ESI:    *value = m_regs._esi;           break;
	case REG_EDI:    *value = m_regs._edi;           break;
	case REG_ESP:    *value = m_regs._esp;           break;
	case REG_EBP:    *value = m_regs._ebp;           break;
	case REG_CS:     *value = m_regs._cs.selector;   break;
	case REG_SS:     *value = m_regs._ss.selector;   break;
	case REG_DS:     *value = m_regs._ds.selector;   break;
	case REG_ES:     *value = m_regs._es.selector;   break;
	case REG_FS:     *value = m_regs._fs.selector;   break;
	case REG_GS:     *value = m_regs._gs.selector;   break;
	case REG_TR:     *value = m_regs._tr.selector;   break;
	case REG_CR0:    *value = m_regs._cr0;           break;
	case REG_CR2:    *value = m_regs._cr2;           break;
	case REG_CR3:    *value = m_regs._cr3;           break;
	case REG_CR4:    *value = m_regs._cr4;           break;
	default:                                         return -1;
	}

	return 0;
}

int HaxmCpu::RegWrite(enum CpuReg reg, uint32_t value) {
	int status = RefreshRegisters(false);
	if (status) { return status; }

	switch (reg) {
	case REG_EIP:    m_regs._eip = value;                      break;
	case REG_EFLAGS: m_regs._eflags = (value | 0x2) & ~0x8028; break;
	case REG_EAX:    m_regs._eax = value;                      break;
	case REG_ECX:    m_regs._ecx = value;                      break;
	case REG_EDX:    m_regs._edx = value;                      break;
	case REG_EBX:    m_regs._ebx = value;                      break;
	case REG_ESI:    m_regs._esi = value;                      break;
	case REG_EDI:    m_regs._edi = value;                      break;
	case REG_ESP:    m_regs._esp = value;                      break;
	case REG_EBP:    m_regs._ebp = value;                      break;
	case REG_CS:     LoadSegmentSelector(value, &m_regs._cs);  break;
	case REG_SS:     LoadSegmentSelector(value, &m_regs._ss);  break;
	case REG_DS:     LoadSegmentSelector(value, &m_regs._ds);  break;
	case REG_ES:     LoadSegmentSelector(value, &m_regs._es);  break;
	case REG_FS:     LoadSegmentSelector(value, &m_regs._fs);  break;
	case REG_GS:     LoadSegmentSelector(value, &m_regs._gs);  break;
	case REG_TR:     LoadSegmentSelector(value, &m_regs._tr);  break;
	case REG_CR0:    m_regs._cr0 = value;                      break;
	case REG_CR2:    m_regs._cr2 = value;                      break;
	case REG_CR3:    m_regs._cr3 = value;                      break;
	case REG_CR4:    m_regs._cr4 = value;                      break;
	default:                                                   return -1;
	}

	m_regsChanged = true;

	return 0;
}

int HaxmCpu::GetGDT(uint32_t *addr, uint32_t *size) {
	int status = RefreshRegisters(false);
	if (status) { return status; }

	*addr = m_regs._gdt.base;
	*size = m_regs._gdt.limit;

	return 0;
}

int HaxmCpu::SetGDT(uint32_t addr, uint32_t size) {
	int status = RefreshRegisters(false);
	if (status) { return status; }

	m_regs._gdt.base = addr;
	m_regs._gdt.limit = size;

	m_regsChanged = true;

	return 0;
}

int HaxmCpu::GetIDT(uint32_t *addr, uint32_t *size) {
	int status = RefreshRegisters(false);
	if (status) { return status; }

	*addr = m_regs._idt.base;
	*size = m_regs._idt.limit;

	return 0;
}

int HaxmCpu::SetIDT(uint32_t addr, uint32_t size) {
	int status = RefreshRegisters(false);
	if (status) { return status; }

	m_regs._idt.base = addr;
	m_regs._idt.limit = size;

	m_regsChanged = true;

	return 0;
}

int HaxmCpu::ReadMSR(uint32_t reg, uint64_t *value) {
	hax_msr_data msrs;
	msrs.nr_msr = 1;
	msrs.entries[0].entry = reg;
	auto regStatus = m_vcpu->GetMSRs(&msrs);
	if (regStatus) { return regStatus; }

	*value = msrs.entries[0].value;
	
	return 0;
}

int HaxmCpu::WriteMSR(uint32_t reg, uint64_t value) {
	hax_msr_data msrs;
	msrs.nr_msr = 1;
	msrs.entries[0].entry = reg;
	msrs.entries[0].value = value;
	auto regStatus = m_vcpu->SetMSRs(&msrs);
	if (regStatus) { return regStatus; }

	return msrs.done ? 0 : -1;
}

int HaxmCpu::InvalidateTLBEntry(uint32_t addr) {
	// TODO: implement
	return 0;
}

int HaxmCpu::HandleIO(uint8_t df, uint16_t port, uint8_t direction, uint16_t size, uint16_t count, uint8_t *buffer) {
	uint8_t *ptr;
	if (df) {
		ptr = buffer + size * count - size;
	}
	else {
		ptr = buffer;
	}
	
	for (uint16_t i = 0; i < count; i++) {
        if (direction == HAX_IO_OUT) {
            uint32_t value;
            switch (size) {
            case 1: value = *ptr; break;
            case 2: value = *reinterpret_cast<uint16_t *>(ptr); break;
            case 4: value = *reinterpret_cast<uint32_t *>(ptr); break;
            default: assert(0); // should not happen
            }
            m_ioMapper->IOWrite(port, value, size);
        }
        else {
            m_ioMapper->IORead(port, reinterpret_cast<uint32_t*>(buffer), size);
        }

        if (df) {
            ptr -= size;
        }
        else {
            ptr += size;
        }
	}

	return 0;
}

int HaxmCpu::HandleFastMMIO(struct hax_fastmmio *info) {
	if (info->direction < 2) {
        if (info->direction == HAX_IO_IN) {
            m_ioMapper->MMIOWrite(info->gpa, (uint32_t)info->value, info->size);
        }
        else {
            m_ioMapper->MMIORead(info->gpa, (uint32_t*)&info->value, info->size);
        }
	}
	else {
		// HAX API v4 supports transferring data between two MMIO addresses,
		// info->gpa and info->gpa2 (instructions such as MOVS require this):
		//  info->direction == 2: gpa ==> gpa2

        uint32_t value;
        m_ioMapper->MMIORead(info->gpa, &value, info->size);
        m_ioMapper->MMIOWrite(info->gpa2, value, info->size);
	}
	return 0;
}

int HaxmCpu::RefreshRegisters(bool refreshFPU) {
	if (m_regsDirty) {
		auto regStatus = m_vcpu->GetRegisters(&m_regs);
		if (regStatus) { return regStatus; }

		if (refreshFPU) {
			m_vcpu->GetFPURegisters(&m_fpuRegs);
		}

		m_regsDirty = false;
	}
	return 0;
}

int HaxmCpu::LoadSegmentSelector(uint16_t selector, segment_desc_t *segment) {
	// Set basic register data
	segment->selector = selector;

	// Get GDT entry from memory
	GDTEntry gdtEntry;
	VMemRead(m_regs._gdt.base + selector, sizeof(GDTEntry), &gdtEntry);

	// Fill in the rest of the CS info with data from the GDT entry
	segment->ar = gdtEntry.data.access | (gdtEntry.data.flags << 12);
	segment->base = gdtEntry.GetBase();
	segment->limit = gdtEntry.GetLimit();

	m_regsChanged = true;
	
	return 0;
}

void HaxmCpu::InjectPendingInterrupt() {
    // If there aren't enough credits or there are no pending interrupts, get out
    if (m_interruptHandlerCredits < kInterruptHandlerCost || m_pendingInterrupts.size() == 0) {
        return;
    }

    // Spend the credits and handle one interrupt
    m_interruptHandlerCredits -= kInterruptHandlerCost;

    // Acquire the pending interrupts mutex
    std::lock_guard<std::mutex> guard(m_pendingInterruptsMutex);

    // Dequeue the next interrupt vector
    uint8_t vector = m_pendingInterrupts.front();
    m_pendingInterrupts.pop();

    // Clear the bit in the pending interrupts bitmap
    Bitmap64Clear(&m_pendingInterruptsBitmap, vector);

    // Inject the interrupt into the VCPU
    m_vcpu->Interrupt(vector);
    return;
}

}
