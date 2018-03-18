
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

    auto kvmRun = m_vcpu->kvmRun();

    if(kvmRun->ready_for_interrupt_injection) {
        InjectPendingInterrupt();
    } else if(!m_pendingInterrupts.empty()) {
        kvmRun->request_interrupt_window = 0x1;
    }

    auto status = m_vcpu->Run();

    if(status == KVMVCPUS_RUN_FAILED) {
        return -1;
    }

    switch(kvmRun->exit_reason) {
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

    return 0;
}

int KvmCpu::StepImpl(uint64_t num_instructions) {
    return 0;
}

InterruptResult KvmCpu::InterruptImpl(uint8_t vector) {
    std::lock_guard<std::mutex> guard(m_pendingInterruptsMutex);

    if(!Bitmap64IsSet(m_pendingInterruptsBitmap, vector)) {
        m_pendingInterrupts.push(vector);
        Bitmap64Set(&m_pendingInterruptsBitmap, vector);
    } else {
        m_skippedInterrupts[vector]++;
    }

    return INTR_SUCCESS;
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


int KvmCpu::HandleIO(uint8_t direction, uint16_t port, uint8_t size, uint32_t count, uint64_t dataOffset) {
    uint8_t *ptr;
    if(count > 1) {
        ptr = (uint8_t*)((((uint64_t)m_vcpu->kvmRun()) + dataOffset) + size * count - size);
    } else {
        ptr = (uint8_t*)(((uint64_t)m_vcpu->kvmRun()) + dataOffset);
    }

    for(uint16_t i = 0; i < count; i++) {
        if(direction == KVM_EXIT_IO_OUT) {
            uint32_t value;
            switch(size) {
                case 1: value = *ptr; break;
                case 2: value = *reinterpret_cast<uint16_t*>(ptr); break;
                case 4: value = *reinterpret_cast<uint32_t*>(ptr); break;
                default: assert(0);
            }
            m_ioMapper->IOWrite(port, value, size);
        } else {
            m_ioMapper->IORead(port, (uint32_t*)(((uint64_t)m_vcpu->kvmRun()) + dataOffset), size);
        }

        if(count > 0) {
            ptr -= size;
        } else {
            ptr += size;
        }
    }
    return 0;
}

int KvmCpu::HandleMMIO(uint32_t physAddress, uint32_t *data, uint8_t size, uint8_t isWrite) {
    if(isWrite) {
        m_ioMapper->MMIOWrite(physAddress, *data, size);
    } else {
        m_ioMapper->MMIORead(physAddress, data, size);
    }
    return 0;
}

void KvmCpu::InjectPendingInterrupt() {
    if(m_interruptHandlerCredits < kInterruptHandlerMaxCredits) {
        m_interruptHandlerCredits += kInterruptHandlerIncrement;
    }

    if(m_interruptHandlerCredits < kInterruptHandlerCost || m_pendingInterrupts.size() == 0) {
        return;
    }

    std::lock_guard<std::mutex> guard(m_pendingInterruptsMutex);

    uint8_t vector = m_pendingInterrupts.front();

    if(m_vcpu->Interrupt(vector) == KVMVCPUS_INTERRUPT_FAILED) {
        m_vcpu->kvmRun()->request_interrupt_window = 1;
        return;
    }

    m_interruptHandlerCredits -= kInterruptHandlerCost;

    m_pendingInterrupts.pop();

    Bitmap64Clear(&m_pendingInterruptsBitmap, vector);

    return;
}

}