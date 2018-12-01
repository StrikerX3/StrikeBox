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
}

HaxmCpu::~HaxmCpu() {
	if (m_haxm != nullptr) {
		// Deleting this object will automatically delete the VM and VCPU
		delete m_haxm;
		m_haxm = nullptr;
	}
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

    return HandleExecResult(status);
}

int HaxmCpu::StepImpl() {
    // Update CPU state if registers were modified
    if (m_regsChanged) {
        m_vcpu->SetRegisters(&m_regs);
        m_regsChanged = false;
    }
    if (m_fpuRegsChanged) {
        m_vcpu->SetFPURegisters(&m_fpuRegs);
        m_fpuRegsChanged = false;
    }

    // Run one instruction
    auto status = m_vcpu->Step();

    return HandleExecResult(status);
}

int HaxmCpu::HandleExecResult(HaxmVCPUStatus status) {
    // Mark registers as dirty
    m_regsDirty = true;
    m_fpuRegsDirty = true;

    // Check VM exit status
    if (status == HXVCPUS_FAILED) {
        return -1;
    }

    auto tunnel = m_vcpu->Tunnel();
  
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
    case HAX_EXIT_DEBUG:                                                       // A breakpoint was hit
    {
        // Determine if it was a software or hardware breakpoint
        if (tunnel->debug.dr6 & 0xf) {
            m_exitInfo.reason = CPU_EXIT_HW_BREAKPOINT;
        }
        else {
            m_exitInfo.reason = CPU_EXIT_SW_BREAKPOINT;
        }
        break;
    }
    }

    return 0;
}

InterruptResult HaxmCpu::InterruptImpl(uint8_t vector) {
	return INTR_SUCCESS;
}

int HaxmCpu::MemMapSubregion(MemoryRegion *subregion) {
	switch (subregion->m_type) {
	case MEM_REGION_MMIO: {
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

int HaxmCpu::EnableSoftwareBreakpoints(bool enable) {
    auto status = m_vcpu->EnableSoftwareBreakpoints(enable);
    if (status != HXVCPUS_SUCCESS) {
        return -1;
    }

    return 0;
}

int HaxmCpu::SetHardwareBreakpoints(HardwareBreakpoints breakpoints) {
    HaxmHardwareBreakpoint bps[4];
    for (int i = 0; i < 4; i++) {
        bps[i].address = breakpoints.bp[0].address;
        bps[i].localEnable = breakpoints.bp[0].localEnable;
        bps[i].globalEnable = breakpoints.bp[0].globalEnable;
        bps[i].trigger = (HaxmHardwareBreakpointTrigger) breakpoints.bp[0].trigger;
        bps[i].length = (HaxmHardwareBreakpointLength) breakpoints.bp[0].length;
    }

    auto status = m_vcpu->SetHardwareBreakpoints(bps);
    if (status != HXVCPUS_SUCCESS) {
        return -1;
    }

    return 0;
}

int HaxmCpu::ClearHardwareBreakpoints() {
    auto status = m_vcpu->ClearHardwareBreakpoints();
    if (status != HXVCPUS_SUCCESS) {
        return -1;
    }

    return 0;
}

bool HaxmCpu::GetBreakpointAddress(uint32_t *address) {
    auto debugTunnel = &m_vcpu->Tunnel()->debug;
    char hardwareBP = debugTunnel->dr6 & 0xF;

    // No breakpoints were hit
    if (hardwareBP == 0 && debugTunnel->rip == 0) {
        return false;
    }

    // Get the appropriate breakpoint address
    auto debug = m_vcpu->Debug();
    /**/ if (hardwareBP & (1 << 0)) *address = debug->dr[0];
    else if (hardwareBP & (1 << 1)) *address = debug->dr[1];
    else if (hardwareBP & (1 << 2)) *address = debug->dr[2];
    else if (hardwareBP & (1 << 3)) *address = debug->dr[3];
    else /************************/ *address = (uint32_t)debugTunnel->rip;

    return true;
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

int HaxmCpu::InjectInterrupt(uint8_t vector) {
    int result = m_vcpu->Interrupt(vector);
    m_vcpu->Tunnel()->request_interrupt_window = 0;
    return result;
}

bool HaxmCpu::CanInjectInterrupt() {
    return m_vcpu->Tunnel()->ready_for_interrupt_injection != 0;
}

void HaxmCpu::RequestInterruptWindow() {
    m_vcpu->Tunnel()->request_interrupt_window = 1;
}

}
