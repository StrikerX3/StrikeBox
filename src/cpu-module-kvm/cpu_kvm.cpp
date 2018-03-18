
#include "cpu_kvm.h"
#include "openxbox/log.h"

#include <fcntl.h>
#include <cassert>
#include <linux/kvm.h>

namespace openxbox {

KvmCpu::KvmCpu() {
    m_kvm = nullptr;
    m_vm = nullptr;
    m_vcpu = nullptr;
}

KvmCpu::~KvmCpu() {
    if(m_kvm != nullptr) {
        // Deleting this will automatically delete the VM and VCPU
        delete m_kvm;
        m_kvm = nullptr;
    }
}

int KvmCpu::InitializeImpl() {
    if(m_kvm == nullptr) {
        m_kvm = new Kvm;

        auto status = m_kvm->Initialize();
        if(status != KVMS_SUCCESS) {
            return status;
        }

        auto vmStatus = m_kvm->CreateVM(&m_vm);
        if(vmStatus != KVMVMS_SUCCESS) {
            delete m_kvm;
            m_kvm = nullptr;
            return vmStatus;
        }

        auto vcpuStatus = m_vm->CreateVCPU(&m_vcpu);
        if(vcpuStatus != KVMVCPUS_SUCCESS) {
            delete m_kvm;
            m_kvm = nullptr;
            return vcpuStatus;
        }
    }

    return 0;
}

int KvmCpu::RunImpl() {
    auto status = m_vcpu->Run();

    if(status == KVMVCPUS_RUN_FAILED) {
        return -1;
    }

    switch(m_vcpu->kvmRun()->exit_reason) {
        case KVM_EXIT_HLT:          m_exitInfo.reason = CPU_EXIT_HLT;       break;
        case KVM_EXIT_IO:           m_exitInfo.reason = CPU_EXIT_NORMAL;    break;
        case KVM_EXIT_MMIO:         m_exitInfo.reason = CPU_EXIT_NORMAL;    break;
        case KVM_EXIT_INTR:         m_exitInfo.reason = CPU_EXIT_NORMAL;    break;
        case KVM_EXIT_FAIL_ENTRY:   m_exitInfo.reason = CPU_EXIT_SHUTDOWN;
            log_error("KVM_EXIT_FAIL_ENTRY. Failure reason: 0x%X\n", m_vcpu->kvmRun()->fail_entry.hardware_entry_failure_reason);
            break;
    }

    return 0;
}

int KvmCpu::StepImpl(uint64_t num_instructions) {
    return 0;
}

InterruptResult KvmCpu::InterruptImpl(uint8_t vector) {
    return *(new InterruptResult);
}

int KvmCpu::MemMapSubregion(MemoryRegion *subregion) {
    log_debug("KvmCpu: Mapping 0x%X bytes to guest memory address 0x%X\n", subregion->m_size, subregion->m_start);

    switch(subregion->m_type) {
        case MEM_REGION_MMIO: {
            // Do nothing - KVM treats all unmapped memory as MMIO.
            return 0;
        }

        case MEM_REGION_NONE: {
            // Shouldn't happen.
            assert(0);
            return -1;
        }

        case MEM_REGION_RAM:
        case MEM_REGION_ROM: {
            auto status = m_vm->MapUserMemoryToGuest(subregion->m_data, subregion->m_size, subregion->m_start);
            if(status) { return status; }
            return 0;
        }
    }
    return -1;
}

int KvmCpu::MemRead(uint32_t addr, uint32_t size, void *value) {
    return 0;
}

int KvmCpu::MemWrite(uint32_t addr, uint32_t size, void *value) {
    return 0;
}

int KvmCpu::RegRead(enum CpuReg reg, uint32_t *value) {
    return 0;
}

int KvmCpu::RegWrite(enum CpuReg reg, uint32_t value) {
    return 0;
}

int KvmCpu::GetGDT(uint32_t *addr, uint32_t *size) {
    return 0;
}

int KvmCpu::SetGDT(uint32_t addr, uint32_t size) {
    return 0;
}

int KvmCpu::GetIDT(uint32_t *addr, uint32_t *size) {
    return 0;
}

int KvmCpu::SetIDT(uint32_t addr, uint32_t size) {
    return 0;
}

int KvmCpu::ReadMSR(uint32_t reg, uint64_t *value) {
    return 0;
}

int KvmCpu::WriteMSR(uint32_t reg, uint64_t value) {
    return 0;
}

int KvmCpu::InvalidateTLBEntry(uint32_t addr) {
    return 0;
}

}