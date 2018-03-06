#include <assert.h>
#include "cpu_haxm.h"

#include "openxbox/log.h"

namespace openxbox {

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
	auto tunnel = m_vcpu->Tunnel();
	switch (tunnel->_exit_status) {
	case HAX_EXIT_HLT: { // The one instruction OpenXBOX depends on
		m_exitInfo.reason = CPU_EXIT_HLT;
		// Make EIP point to the actual HLT instruction
		uint32_t r;
		RegRead(REG_EIP, &r);
		RegWrite(REG_EIP, r - 1);
		break;
	}
	case HAX_EXIT_IO:          m_exitInfo.reason = CPU_EXIT_NORMAL;            // I/O (in / out instructions)
		return HandleIO(tunnel->io._df, tunnel->io._port, tunnel->io._direction, tunnel->io._size, tunnel->io._count, m_vcpu->IOTunnel());
	case HAX_EXIT_MMIO:        m_exitInfo.reason = CPU_EXIT_NORMAL;    return HandleMMIO(tunnel->mmio.gla, tunnel->io._direction);  // MMIO
	case HAX_EXIT_FAST_MMIO:   m_exitInfo.reason = CPU_EXIT_NORMAL;    return HandleFastMMIO((struct hax_fastmmio *)m_vcpu->IOTunnel());  // Fast MMIO
	case HAX_EXIT_INTERRUPT:   m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Let HAXM handle this
	case HAX_EXIT_PAUSED:      m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Let HAXM handle this
	case HAX_EXIT_REALMODE:    m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Real mode is not supported
	case HAX_EXIT_UNKNOWN:     m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // VM failed for an unknown reason
	case HAX_EXIT_STATECHANGE: m_exitInfo.reason = CPU_EXIT_SHUTDOWN;  break;  // The VM is shutting down
	}

    // Inject an interrupt if possible
    if (tunnel->ready_for_interrupt_injection) {
        InjectPendingInterrupt();
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
		// TODO: delegate to I/O mapper
		// port: port number
		// ptr: pointer to data buffer
		// size: size of data buffer
		// direction: read (HAX_IO_OUT) or write (HAX_IO_IN)
	}

	log_warning("I/O unimplemented!   df: %d  port: 0x%04x  direction: %d  size: %d  count: %d\n", df, port, direction, size, count);

	return 0;
}

int HaxmCpu::HandleMMIO(uint32_t physAddress, uint8_t direction) {
	log_warning("MMIO unimplemented!   address: 0x%08x  direction: %d\n", physAddress, direction);
	// TODO: handle MMIO at the given physical address
	// direction: read (HAX_IO_OUT) or write (HAX_IO_IN)
	return 0;
}

int HaxmCpu::HandleFastMMIO(struct hax_fastmmio *info) {
	if (info->direction < 2) {
		log_warning("Fast MMIO unimplemented!   address: 0x%08x  value: 0x%x  size: %d  direction: %d\n", info->gpa, info->value, info->size, info->direction);
		// TODO: handle MMIO at the given physical address
		// info->gpa: physical address
		// info->value: value to read/write
		// info->size: number of bytes to read/write
		// info->direction: read (HAX_IO_OUT) or write (HAX_IO_IN)

		//cpu_physical_memory_rw(info->gpa, (uint8_t *)&info->value, info->size, info->direction);
	}
	else {
		log_warning("Two-way fast MMIO unimplemented!   address1: 0x%08x  address2: 0x%08x  size: %d\n", info->gpa, info->gpa2, info->size);
		// TODO: handle MMIO between two physical addresses
		// info->gpa: physical address to read from
		// info->gpa2: physical address to write to
		// info->size: number of bytes to read/write

		// HAX API v4 supports transferring data between two MMIO addresses,
		// info->gpa and info->gpa2 (instructions such as MOVS require this):
		//  info->direction == 2: gpa ==> gpa2

		uint64_t value;
		//cpu_physical_memory_rw(info->gpa, (uint8_t *)&value, info->size, 0);
		//cpu_physical_memory_rw(info->gpa2, (uint8_t *)&value, info->size, 1);
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
    // Increment the credits available for the interrupt handler
    if (m_interruptHandlerCredits < kInterruptHandlerMaxCredits) {
        m_interruptHandlerCredits += kInterruptHandlerIncrement;
    }

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
