
#include "cpu_kvm.h"
#include "openxbox/log.h"

#include <fcntl.h>

namespace openxbox {

KvmCpu::KvmCpu() {
    m_kvm = nullptr;
}

KvmCpu::~KvmCpu() {
    if(m_kvm != nullptr) {
        delete m_kvm;
        m_kvm = nullptr;
    }
}

int KvmCpu::InitializeImpl() {
    if(m_kvm == nullptr) {
        m_kvm = new Kvm;
    }

    return 0;
}

int KvmCpu::RunImpl() {
    return 0;
}

int KvmCpu::StepImpl(uint64_t num_instructions) {
    return 0;
}

InterruptResult KvmCpu::InterruptImpl(uint8_t vector) {
    return *(new InterruptResult);
}

int KvmCpu::MemMapSubregion(MemoryRegion *subregion) {
    return 0;
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