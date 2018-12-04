#include <assert.h>
#include "cpu_haxm.h"

#include "openxbox/log.h"

namespace openxbox {
namespace cpu {

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

CPUInitStatus HaxmCpu::InitializeImpl() {
    if (m_haxm == nullptr) {
        m_haxm = new Haxm();

        auto status = m_haxm->Initialize();
        if (status != HXS_SUCCESS) {
            return CPUS_INIT_PLATFORM_INIT_FAILED;
        }

        auto vmStatus = m_haxm->CreateVM(&m_vm);
        if (vmStatus != HXVMS_SUCCESS) {
            delete m_haxm;
            m_haxm = nullptr;
            return CPUS_INIT_CREATE_VM_FAILED;
        }

        auto vcpuStatus = m_vm->CreateVCPU(&m_vcpu);
        if (vcpuStatus != HXVCPUS_SUCCESS) {
            delete m_haxm;
            m_haxm = nullptr;
            return CPUS_INIT_CREATE_CPU_FAILED;
        }
    }

    return CPUS_INIT_OK;
}

CPUStatus HaxmCpu::RunImpl() {
    UpdateRegisters();
    auto status = m_vcpu->Run();
    return HandleExecResult(status);
}

CPUStatus HaxmCpu::StepImpl() {
    UpdateRegisters();
    auto status = m_vcpu->Step();
    return HandleExecResult(status);
}

void HaxmCpu::UpdateRegisters() {
    // Update CPU state if registers were modified
    if (m_regsChanged) {
        m_vcpu->SetRegisters(&m_regs);
        m_regsChanged = false;
    }
    if (m_fpuRegsChanged) {
        m_vcpu->SetFPURegisters(&m_fpuRegs);
        m_fpuRegsChanged = false;
    }
}

CPUStatus HaxmCpu::HandleExecResult(HaxmVCPUStatus status) {
    // Mark registers as dirty
    m_regsDirty = true;
    m_fpuRegsDirty = true;

    // Check VM exit status
    if (status == HXVCPUS_FAILED) {
        return CPUS_FAILED;
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

    return CPUS_OK;
}

InterruptResult HaxmCpu::InterruptImpl(uint8_t vector) {
    return INTR_SUCCESS;
}

CPUMemMapStatus HaxmCpu::MemMapSubregion(MemoryRegion *subregion) {
    switch (subregion->m_type) {
    case MEM_REGION_MMIO:
    {
        // All unmapped regions in a HAXM VM are MMIO, no need to allocate
        return CPUS_MMAP_OK;
    }
    case MEM_REGION_NONE:
    {
        // This should not happen
        assert(0);
        return CPUS_MMAP_INVALID_TYPE;
    }
    case MEM_REGION_RAM:
    case MEM_REGION_ROM:
    {
        // Region is either RAM or ROM, map it accordingly
        HaxmVMMemoryType memType = subregion->m_type == MEM_REGION_RAM ? HXVM_MEM_RAM : HXVM_MEM_ROM;
        auto status = m_vm->AllocateMemory(subregion->m_data, subregion->m_size, subregion->m_start, memType);
        switch (status) {
        case HXVMS_SUCCESS: return CPUS_MMAP_OK;
        case HXVMS_MEM_MISALIGNED: return CPUS_MMAP_MEMORY_ADDR_MISALIGNED;
        case HXVMS_MEMSIZE_MISALIGNED: return CPUS_MMAP_MEMORY_SIZE_MISALIGNED;
        case HXVMS_ALLOC_MEM_FAILED: return CPUS_MMAP_MAPPING_FAILED;
        case HXVMS_SET_MEM_FAILED: return CPUS_MMAP_MAPPING_FAILED;
        default: return CPUS_MMAP_UNHANDLED_ERROR;
        }
    }
    default:
        // This should not happen
        return CPUS_MMAP_INVALID_TYPE;
    }
}

#define REFRESH_REGISTERS do { \
    auto status = RefreshRegisters(false); \
    if (status) { \
        return status; \
    } \
} while (0)

CPUOperationStatus HaxmCpu::RegRead(enum CpuReg reg, uint32_t *value) {
    REFRESH_REGISTERS;

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
    default:                                         return CPUS_OP_INVALID_REGISTER;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::RegWrite(enum CpuReg reg, uint32_t value) {
    REFRESH_REGISTERS;

    switch (reg) {
    case REG_EIP:    m_regs._eip = value;                                break;
    case REG_EFLAGS: m_regs._eflags = (value | 0x2) & ~0x8028;           break;
    case REG_EAX:    m_regs._eax = value;                                break;
    case REG_ECX:    m_regs._ecx = value;                                break;
    case REG_EDX:    m_regs._edx = value;                                break;
    case REG_EBX:    m_regs._ebx = value;                                break;
    case REG_ESI:    m_regs._esi = value;                                break;
    case REG_EDI:    m_regs._edi = value;                                break;
    case REG_ESP:    m_regs._esp = value;                                break;
    case REG_EBP:    m_regs._ebp = value;                                break;
    case REG_CS:     LoadSegmentSelector((uint16_t)value, &m_regs._cs);  break;
    case REG_SS:     LoadSegmentSelector((uint16_t)value, &m_regs._ss);  break;
    case REG_DS:     LoadSegmentSelector((uint16_t)value, &m_regs._ds);  break;
    case REG_ES:     LoadSegmentSelector((uint16_t)value, &m_regs._es);  break;
    case REG_FS:     LoadSegmentSelector((uint16_t)value, &m_regs._fs);  break;
    case REG_GS:     LoadSegmentSelector((uint16_t)value, &m_regs._gs);  break;
    case REG_TR:     LoadSegmentSelector((uint16_t)value, &m_regs._tr);  break;
    case REG_CR0:    m_regs._cr0 = value;                                break;
    case REG_CR2:    m_regs._cr2 = value;                                break;
    case REG_CR3:    m_regs._cr3 = value;                                break;
    case REG_CR4:    m_regs._cr4 = value;                                break;
    default:                                                             return CPUS_OP_INVALID_REGISTER;
    }

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::RegRead(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    REFRESH_REGISTERS;

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP:    values[i] = m_regs._eip;           break;
        case REG_EFLAGS: values[i] = m_regs._eflags;        break;
        case REG_EAX:    values[i] = m_regs._eax;           break;
        case REG_ECX:    values[i] = m_regs._ecx;           break;
        case REG_EDX:    values[i] = m_regs._edx;           break;
        case REG_EBX:    values[i] = m_regs._ebx;           break;
        case REG_ESI:    values[i] = m_regs._esi;           break;
        case REG_EDI:    values[i] = m_regs._edi;           break;
        case REG_ESP:    values[i] = m_regs._esp;           break;
        case REG_EBP:    values[i] = m_regs._ebp;           break;
        case REG_CS:     values[i] = m_regs._cs.selector;   break;
        case REG_SS:     values[i] = m_regs._ss.selector;   break;
        case REG_DS:     values[i] = m_regs._ds.selector;   break;
        case REG_ES:     values[i] = m_regs._es.selector;   break;
        case REG_FS:     values[i] = m_regs._fs.selector;   break;
        case REG_GS:     values[i] = m_regs._gs.selector;   break;
        case REG_TR:     values[i] = m_regs._tr.selector;   break;
        case REG_CR0:    values[i] = m_regs._cr0;           break;
        case REG_CR2:    values[i] = m_regs._cr2;           break;
        case REG_CR3:    values[i] = m_regs._cr3;           break;
        case REG_CR4:    values[i] = m_regs._cr4;           break;
        default:                                            return CPUS_OP_INVALID_REGISTER;
        }
    }

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::RegWrite(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    REFRESH_REGISTERS;

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP:    m_regs._eip = values[i];                      break;
        case REG_EFLAGS: m_regs._eflags = (values[i] | 0x2) & ~0x8028; break;
        case REG_EAX:    m_regs._eax = values[i];                      break;
        case REG_ECX:    m_regs._ecx = values[i];                      break;
        case REG_EDX:    m_regs._edx = values[i];                      break;
        case REG_EBX:    m_regs._ebx = values[i];                      break;
        case REG_ESI:    m_regs._esi = values[i];                      break;
        case REG_EDI:    m_regs._edi = values[i];                      break;
        case REG_ESP:    m_regs._esp = values[i];                      break;
        case REG_EBP:    m_regs._ebp = values[i];                      break;
        case REG_CS:     LoadSegmentSelector(values[i], &m_regs._cs);  break;
        case REG_SS:     LoadSegmentSelector(values[i], &m_regs._ss);  break;
        case REG_DS:     LoadSegmentSelector(values[i], &m_regs._ds);  break;
        case REG_ES:     LoadSegmentSelector(values[i], &m_regs._es);  break;
        case REG_FS:     LoadSegmentSelector(values[i], &m_regs._fs);  break;
        case REG_GS:     LoadSegmentSelector(values[i], &m_regs._gs);  break;
        case REG_TR:     LoadSegmentSelector(values[i], &m_regs._tr);  break;
        case REG_CR0:    m_regs._cr0 = values[i];                      break;
        case REG_CR2:    m_regs._cr2 = values[i];                      break;
        case REG_CR3:    m_regs._cr3 = values[i];                      break;
        case REG_CR4:    m_regs._cr4 = values[i];                      break;
        default:                                                       return CPUS_OP_INVALID_REGISTER;
        }
    }

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::GetGDT(uint32_t *addr, uint32_t *size) {
    REFRESH_REGISTERS;

    *addr = m_regs._gdt.base;
    *size = m_regs._gdt.limit;

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::SetGDT(uint32_t addr, uint32_t size) {
    REFRESH_REGISTERS;

    m_regs._gdt.base = addr;
    m_regs._gdt.limit = size;

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::GetIDT(uint32_t *addr, uint32_t *size) {
    REFRESH_REGISTERS;

    *addr = m_regs._idt.base;
    *size = m_regs._idt.limit;

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::SetIDT(uint32_t addr, uint32_t size) {
    REFRESH_REGISTERS;

    m_regs._idt.base = addr;
    m_regs._idt.limit = size;

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::EnableSoftwareBreakpoints(bool enable) {
    auto status = m_vcpu->EnableSoftwareBreakpoints(enable);
    if (status != HXVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::SetHardwareBreakpoints(HardwareBreakpoints breakpoints) {
    HaxmHardwareBreakpoint bps[4];
    for (int i = 0; i < 4; i++) {
        bps[i].address = breakpoints.bp[0].address;
        bps[i].localEnable = breakpoints.bp[0].localEnable;
        bps[i].globalEnable = breakpoints.bp[0].globalEnable;
        bps[i].trigger = (HaxmHardwareBreakpointTrigger)breakpoints.bp[0].trigger;
        bps[i].length = (HaxmHardwareBreakpointLength)breakpoints.bp[0].length;
    }

    auto status = m_vcpu->SetHardwareBreakpoints(bps);
    if (status != HXVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::ClearHardwareBreakpoints() {
    auto status = m_vcpu->ClearHardwareBreakpoints();
    if (status != HXVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus HaxmCpu::GetBreakpointAddress(uint32_t *address) {
    auto debugTunnel = &m_vcpu->Tunnel()->debug;
    char hardwareBP = debugTunnel->dr6 & 0xF;

    // No breakpoints were hit
    if (hardwareBP == 0 && debugTunnel->rip == 0) {
        return CPUS_OP_BREAKPOINT_NEVER_HIT;
    }

    // Get the appropriate breakpoint address
    auto debug = m_vcpu->Debug();
    /**/ if (hardwareBP & (1 << 0)) *address = debug->dr[0];
    else if (hardwareBP & (1 << 1)) *address = debug->dr[1];
    else if (hardwareBP & (1 << 2)) *address = debug->dr[2];
    else if (hardwareBP & (1 << 3)) *address = debug->dr[3];
    else /************************/ *address = (uint32_t)debugTunnel->rip;

    return CPUS_OP_OK;
}

CPUStatus HaxmCpu::HandleIO(uint8_t df, uint16_t port, uint8_t direction, uint16_t size, uint16_t count, uint8_t *buffer) {
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
            m_ioMapper->IORead(port, reinterpret_cast<uint32_t*>(ptr), size);
        }

        if (df) {
            ptr -= size;
        }
        else {
            ptr += size;
        }
    }

    return CPUS_OK;
}

CPUStatus HaxmCpu::HandleFastMMIO(struct hax_fastmmio *info) {
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
    return CPUS_OK;
}

CPUOperationStatus HaxmCpu::RefreshRegisters(bool refreshFPU) {
    if (m_regsDirty) {
        if (m_vcpu->GetRegisters(&m_regs) != HXVCPUS_SUCCESS) {
            return CPUS_OP_FAILED;
        }

        if (refreshFPU) {
            m_vcpu->GetFPURegisters(&m_fpuRegs);
        }

        m_regsDirty = false;
    }
    return CPUS_OP_OK;
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

CPUOperationStatus HaxmCpu::InjectInterrupt(uint8_t vector) {
    auto status = m_vcpu->Interrupt(vector);
    if (status != HXVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }
    m_vcpu->Tunnel()->request_interrupt_window = 0;
    return CPUS_OP_OK;
}

bool HaxmCpu::CanInjectInterrupt() {
    return m_vcpu->Tunnel()->ready_for_interrupt_injection != 0;
}

void HaxmCpu::RequestInterruptWindow() {
    m_vcpu->Tunnel()->request_interrupt_window = 1;
}

}
}
