
#include "cpu_kvm.h"
#include "openxbox/log.h"

#include <fcntl.h>
#include <cassert>
#include <linux/kvm.h>
#include <openxbox/gdt.h>

namespace openxbox {
namespace cpu {

KvmCpu::KvmCpu() {
    m_kvm = nullptr;
    m_vm = nullptr;
    m_vcpu = nullptr;
}

KvmCpu::~KvmCpu() {
    if (m_kvm != nullptr) {
        // Deleting this will automatically delete the VM and VCPU
        delete m_kvm;
        m_kvm = nullptr;
    }
}

CPUInitStatus KvmCpu::InitializeImpl() {
    if (m_kvm == nullptr) {
        m_kvm = new Kvm;

        auto status = m_kvm->Initialize();
        if (status != KVMS_SUCCESS) {
            return CPUS_INIT_PLATFORM_INIT_FAILED;
        }

        auto vmStatus = m_kvm->CreateVM(&m_vm);
        if (vmStatus != KVMVMS_SUCCESS) {
            delete m_kvm;
            m_kvm = nullptr;
            return CPUS_INIT_CREATE_VM_FAILED;
        }

        auto vcpuStatus = m_vm->CreateVCPU(&m_vcpu);
        if (vcpuStatus != KVMVCPUS_SUCCESS) {
            delete m_kvm;
            m_kvm = nullptr;
            return CPUS_INIT_CREATE_CPU_FAILED;
        }
    }

    return CPUS_INIT_OK;
}

CPUStatus KvmCpu::RunImpl() {
    UpdateRegisters();
    auto status = m_vcpu->Run();
    return HandleExecResult(status);
}

void KvmCpu::UpdateRegisters() {
    // Update registers if they've been changed
    if (m_regsChanged) {
        m_vcpu->SetRegisters(m_regs);
        m_vcpu->SetSRegisters(m_sregs);
        m_regsChanged = false;
    }
    if (m_fpuRegsChanged) {
        m_vcpu->SetFPURegisters(m_fpuRegs);
        m_fpuRegsChanged = false;
    }
}

CPUStatus KvmCpu::HandleExecResult(KvmVCPUStatus status) {
    // Mark the registers as dirty
    m_regsDirty = true;
    m_fpuRegsDirty = true;

    // Check VM status
    if (status == KVMVCPUS_RUN_FAILED) {
        return CPUS_FAILED;
    }

    auto kvmRun = m_vcpu->kvmRun();

    // Handle exit status using tunnel
    switch (kvmRun->exit_reason) {
    case KVM_EXIT_HLT:          m_exitInfo.reason = CPU_EXIT_HLT;       break;
    case KVM_EXIT_IO:           m_exitInfo.reason = CPU_EXIT_NORMAL;
        HandleIO(kvmRun->io.direction, kvmRun->io.port, kvmRun->io.size, kvmRun->io.count, kvmRun->io.data_offset);
        break;
    case KVM_EXIT_MMIO:         m_exitInfo.reason = CPU_EXIT_NORMAL;
        HandleMMIO((uint32_t)kvmRun->mmio.phys_addr, (uint32_t*)kvmRun->mmio.data, (uint8_t)kvmRun->mmio.len, kvmRun->mmio.is_write);
        break;
    case KVM_EXIT_INTR:         m_exitInfo.reason = CPU_EXIT_NORMAL;    break;
    case KVM_EXIT_FAIL_ENTRY:   m_exitInfo.reason = CPU_EXIT_SHUTDOWN;
        log_error("KVM_EXIT_FAIL_ENTRY. Failure reason: 0x%X\n", m_vcpu->kvmRun()->fail_entry.hardware_entry_failure_reason);
        break;
    }

    return CPUS_OK;
}


InterruptResult KvmCpu::InterruptImpl(uint8_t vector) {
    return INTR_SUCCESS;
}

CPUMemMapStatus KvmCpu::MemMapSubregion(MemoryRegion *subregion) {
    log_debug("KvmCpu: Mapping 0x%X bytes to guest memory address 0x%X\n", subregion->m_size, subregion->m_start);

    switch (subregion->m_type) {
    case MEM_REGION_MMIO:
    {
        // Do nothing - KVM treats all unmapped memory as MMIO
        return CPUS_MMAP_OK;
    }

    case MEM_REGION_NONE:
    {
        // Shouldn't happen
        assert(0);
        return CPUS_MMAP_INVALID_TYPE;
    }

    case MEM_REGION_RAM:
    case MEM_REGION_ROM:
    {
        auto status = m_vm->MapUserMemoryToGuest(subregion->m_data, subregion->m_size, subregion->m_start);
        switch (status) {
        case KVMVMS_SUCCESS: return CPUS_MMAP_OK;
        case KVMVMS_MEM_MISALIGNED: return CPUS_MMAP_MEMORY_ADDR_MISALIGNED;
        case KVMVMS_MEMSIZE_MISALIGNED: return CPUS_MMAP_MEMORY_SIZE_MISALIGNED;
        case KVMVMS_MEM_ERROR: return CPUS_MMAP_MAPPING_FAILED;
            return CPUS_MMAP_UNHANDLED_ERROR;
        case KVMVMS_CREATE_FAILED:
            // Shouldn't happen
            return CPUS_MMAP_UNHANDLED_ERROR;
        }
    }
    default:
        // Shouldn't happen
        return CPUS_MMAP_INVALID_TYPE;
    }
}

#define REFRESH_REGISTERS do { \
    auto status = RefreshRegisters(false); \
    if (status) { \
        return status; \
    } \
} while (0)

CPUOperationStatus KvmCpu::RegRead(enum CpuReg reg, uint32_t *value) {
    REFRESH_REGISTERS;

    switch (reg) {
    case REG_EIP:       *value = (uint32_t)m_regs.rip;      break;
    case REG_EFLAGS:    *value = (uint32_t)m_regs.rflags;   break;
    case REG_EAX:       *value = (uint32_t)m_regs.rax;      break;
    case REG_ECX:       *value = (uint32_t)m_regs.rcx;      break;
    case REG_EDX:       *value = (uint32_t)m_regs.rdx;      break;
    case REG_EBX:       *value = (uint32_t)m_regs.rbx;      break;
    case REG_ESI:       *value = (uint32_t)m_regs.rsi;      break;
    case REG_EDI:       *value = (uint32_t)m_regs.rdi;      break;
    case REG_ESP:       *value = (uint32_t)m_regs.rsp;      break;
    case REG_EBP:       *value = (uint32_t)m_regs.rbp;      break;
    case REG_CS:        *value = m_sregs.cs.selector;       break;
    case REG_SS:        *value = m_sregs.ss.selector;       break;
    case REG_DS:        *value = m_sregs.ds.selector;       break;
    case REG_ES:        *value = m_sregs.es.selector;       break;
    case REG_FS:        *value = m_sregs.fs.selector;       break;
    case REG_GS:        *value = m_sregs.gs.selector;       break;
    case REG_TR:        *value = m_sregs.tr.selector;       break;
    case REG_CR0:       *value = (uint32_t)m_sregs.cr0;     break;
    case REG_CR2:       *value = (uint32_t)m_sregs.cr2;     break;
    case REG_CR3:       *value = (uint32_t)m_sregs.cr3;     break;
    case REG_CR4:       *value = (uint32_t)m_sregs.cr4;     break;
    default:                                                return CPUS_OP_INVALID_REGISTER;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::RegWrite(enum CpuReg reg, uint32_t value) {
    REFRESH_REGISTERS;

    switch (reg) {
    case REG_EIP:       m_regs.rip = value;                                 break;
    case REG_EFLAGS:    m_regs.rflags = value;                              break;
    case REG_EAX:       m_regs.rax = value;                                 break;
    case REG_ECX:       m_regs.rcx = value;                                 break;
    case REG_EDX:       m_regs.rdx = value;                                 break;
    case REG_EBX:       m_regs.rbx = value;                                 break;
    case REG_ESI:       m_regs.rsi = value;                                 break;
    case REG_EDI:       m_regs.rdi = value;                                 break;
    case REG_ESP:       m_regs.rsp = value;                                 break;
    case REG_EBP:       m_regs.rbp = value;                                 break;
    case REG_CS:        LoadSegmentSelector((uint16_t)value, &m_sregs.cs);  break;
    case REG_SS:        LoadSegmentSelector((uint16_t)value, &m_sregs.ss);  break;
    case REG_DS:        LoadSegmentSelector((uint16_t)value, &m_sregs.ds);  break;
    case REG_ES:        LoadSegmentSelector((uint16_t)value, &m_sregs.es);  break;
    case REG_FS:        LoadSegmentSelector((uint16_t)value, &m_sregs.fs);  break;
    case REG_GS:        LoadSegmentSelector((uint16_t)value, &m_sregs.gs);  break;
    case REG_TR:        LoadSegmentSelector((uint16_t)value, &m_sregs.tr);  break;
    case REG_CR0:       m_sregs.cr0 = value;                                break;
    case REG_CR2:       m_sregs.cr2 = value;                                break;
    case REG_CR3:       m_sregs.cr3 = value;                                break;
    case REG_CR4:       m_sregs.cr4 = value;                                break;
    default:                                                                return CPUS_OP_INVALID_REGISTER;
    }

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::RegRead(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    REFRESH_REGISTERS;

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP:       values[i] = (uint32_t)m_regs.rip;      break;
        case REG_EFLAGS:    values[i] = (uint32_t)m_regs.rflags;   break;
        case REG_EAX:       values[i] = (uint32_t)m_regs.rax;      break;
        case REG_ECX:       values[i] = (uint32_t)m_regs.rcx;      break;
        case REG_EDX:       values[i] = (uint32_t)m_regs.rdx;      break;
        case REG_EBX:       values[i] = (uint32_t)m_regs.rbx;      break;
        case REG_ESI:       values[i] = (uint32_t)m_regs.rsi;      break;
        case REG_EDI:       values[i] = (uint32_t)m_regs.rdi;      break;
        case REG_ESP:       values[i] = (uint32_t)m_regs.rsp;      break;
        case REG_EBP:       values[i] = (uint32_t)m_regs.rbp;      break;
        case REG_CS:        values[i] = m_sregs.cs.selector;       break;
        case REG_SS:        values[i] = m_sregs.ss.selector;       break;
        case REG_DS:        values[i] = m_sregs.ds.selector;       break;
        case REG_ES:        values[i] = m_sregs.es.selector;       break;
        case REG_FS:        values[i] = m_sregs.fs.selector;       break;
        case REG_GS:        values[i] = m_sregs.gs.selector;       break;
        case REG_TR:        values[i] = m_sregs.tr.selector;       break;
        case REG_CR0:       values[i] = (uint32_t)m_sregs.cr0;     break;
        case REG_CR2:       values[i] = (uint32_t)m_sregs.cr2;     break;
        case REG_CR3:       values[i] = (uint32_t)m_sregs.cr3;     break;
        case REG_CR4:       values[i] = (uint32_t)m_sregs.cr4;     break;
        default:                                                   return CPUS_OP_INVALID_REGISTER;
        }
    }

    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::RegWrite(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    REFRESH_REGISTERS;

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP:       m_regs.rip = values[i];                                 break;
        case REG_EFLAGS:    m_regs.rflags = values[i];                              break;
        case REG_EAX:       m_regs.rax = values[i];                                 break;
        case REG_ECX:       m_regs.rcx = values[i];                                 break;
        case REG_EDX:       m_regs.rdx = values[i];                                 break;
        case REG_EBX:       m_regs.rbx = values[i];                                 break;
        case REG_ESI:       m_regs.rsi = values[i];                                 break;
        case REG_EDI:       m_regs.rdi = values[i];                                 break;
        case REG_ESP:       m_regs.rsp = values[i];                                 break;
        case REG_EBP:       m_regs.rbp = values[i];                                 break;
        case REG_CS:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.cs);  break;
        case REG_SS:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.ss);  break;
        case REG_DS:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.ds);  break;
        case REG_ES:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.es);  break;
        case REG_FS:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.fs);  break;
        case REG_GS:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.gs);  break;
        case REG_TR:        LoadSegmentSelector((uint16_t)values[i], &m_sregs.tr);  break;
        case REG_CR0:       m_sregs.cr0 = values[i];                                break;
        case REG_CR2:       m_sregs.cr2 = values[i];                                break;
        case REG_CR3:       m_sregs.cr3 = values[i];                                break;
        case REG_CR4:       m_sregs.cr4 = values[i];                                break;
        default:                                                                    return CPUS_OP_INVALID_REGISTER;
        }
    }

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::GetGDT(uint32_t *addr, uint32_t *size) {
    REFRESH_REGISTERS;

    *addr = (uint32_t)m_sregs.gdt.base;
    *size = m_sregs.gdt.limit;

    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::SetGDT(uint32_t addr, uint32_t size) {
    REFRESH_REGISTERS;

    m_sregs.gdt.base = addr;
    m_sregs.gdt.limit = size;

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::GetIDT(uint32_t *addr, uint32_t *size) {
    REFRESH_REGISTERS;

    *addr = (uint32_t)m_sregs.idt.base;
    *size = m_sregs.idt.limit;
    return CPUS_OP_OK;
}

CPUOperationStatus KvmCpu::SetIDT(uint32_t addr, uint32_t size) {
    REFRESH_REGISTERS;

    m_sregs.idt.base = addr;
    m_sregs.idt.limit = size;

    m_regsChanged = true;

    return CPUS_OP_OK;
}

CPUStatus KvmCpu::HandleIO(uint8_t direction, uint16_t port, uint8_t size, uint32_t count, uint64_t dataOffset) {
    uint8_t *ptr;
    if (count > 1) {
        ptr = (uint8_t*)((((uint64_t)m_vcpu->kvmRun()) + dataOffset) + size * count - size);
    }
    else {
        ptr = (uint8_t*)(((uint64_t)m_vcpu->kvmRun()) + dataOffset);
    }

    for (uint16_t i = 0; i < count; i++) {
        if (direction == KVM_EXIT_IO_OUT) {
            uint32_t value;
            switch (size) {
            case 1: value = *ptr; break;
            case 2: value = *reinterpret_cast<uint16_t*>(ptr); break;
            case 4: value = *reinterpret_cast<uint32_t*>(ptr); break;
            default: assert(0);
            }
            m_ioMapper->IOWrite(port, value, size);
        }
        else {
            m_ioMapper->IORead(port, (uint32_t*)(((uint64_t)m_vcpu->kvmRun()) + dataOffset), size);
        }

        if (count > 0) {
            ptr -= size;
        }
        else {
            ptr += size;
        }
    }
    return CPUS_OK;
}

CPUStatus KvmCpu::HandleMMIO(uint32_t physAddress, uint32_t *data, uint8_t size, uint8_t isWrite) {
    if (isWrite) {
        m_ioMapper->MMIOWrite(physAddress, *data, size);
    }
    else {
        m_ioMapper->MMIORead(physAddress, data, size);
    }
    return CPUS_OK;
}

CPUOperationStatus KvmCpu::RefreshRegisters(bool refreshFPU) {
    if (m_regsDirty) {
        auto regStatus = m_vcpu->GetRegisters(&m_regs);
        auto sRegStatus = m_vcpu->GetSRegisters(&m_sregs);

        if ((regStatus != KVMVCPUS_SUCCESS) ||
            (sRegStatus != KVMVCPUS_SUCCESS)) {
            return CPUS_OP_FAILED;
        }

        if (refreshFPU) {
            m_vcpu->GetFPURegisters(&m_fpuRegs);
        }

        m_regsDirty = false;
    }
    return CPUS_OP_OK;
}

int KvmCpu::LoadSegmentSelector(uint16_t selector, struct kvm_segment *segment) {
    segment->selector = selector;

    GDTEntry gdtEntry;
    VMemRead((uint32_t)(m_sregs.gdt.base + selector), sizeof(GDTEntry), &gdtEntry);

    // Not much on this in KVM documentation. See Intel documentation for a better description.
    // "Intel Software Developer's Manual Combined Volumes: 1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D and 4"
    // "Vol 3C - 24.4 Guest-State Area". Specifically pg. 3770 & 3771
    segment->avl = (uint8_t)(gdtEntry.data.flags & 0x1);        //Availability
    segment->l = (uint8_t)((gdtEntry.data.flags >> 1) & 0x1);   //Long mode
    segment->db = (uint8_t)((gdtEntry.data.flags >> 2) & 0x1);   //Default operation size
    segment->g = (uint8_t)(((gdtEntry.data.flags) >> 3) & 0x1); //Granularity

    segment->present = (uint8_t)((gdtEntry.data.access >> 7) & 0x1);
    segment->dpl = (uint8_t)((gdtEntry.data.access >> 5) & 0x3);
    segment->s = (uint8_t)((gdtEntry.data.access >> 4) & 0x1);
    segment->type = (uint8_t)(gdtEntry.data.access & 0xF);

    segment->base = gdtEntry.GetBase();
    segment->limit = gdtEntry.GetLimit();

    m_regsChanged = true;

    return 0;
}

CPUOperationStatus KvmCpu::InjectInterrupt(uint8_t vector) {
    auto status = m_vcpu->Interrupt(vector);
    if (status != KVMVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }
    m_vcpu->kvmRun()->request_interrupt_window = 0;
    return CPUS_OP_OK;
}

bool KvmCpu::CanInjectInterrupt() {
    return m_vcpu->kvmRun()->ready_for_interrupt_injection != 0;
}

void KvmCpu::RequestInterruptWindow() {
    m_vcpu->kvmRun()->request_interrupt_window = 1;
}

}
}
