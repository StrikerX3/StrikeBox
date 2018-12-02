#pragma once

#include "openxbox/cpu.h"
#include "kvm/kvm.h"

#include <linux/kvm.h>
#include <vector>
#include <queue>
#include <mutex>
#include <openxbox/bitmap.h>

namespace openxbox {

class KvmCpu : public Cpu {

public:
    KvmCpu();
    ~KvmCpu();

    CPUInitStatus InitializeImpl();

    CPUStatus RunImpl();
    InterruptResult InterruptImpl(uint8_t vector);

    CPUMemMapStatus MemMapSubregion(MemoryRegion *subregion);

    CPUOperationStatus RegRead(enum CpuReg reg, uint32_t *value);
    CPUOperationStatus RegWrite(enum CpuReg reg, uint32_t value);

    CPUOperationStatus RegRead(enum CpuReg regs[], uint32_t values[], uint8_t numRegs) override;
    CPUOperationStatus RegWrite(enum CpuReg regs[], uint32_t values[], uint8_t numRegs) override;

    CPUOperationStatus GetGDT(uint32_t *addr, uint32_t *size);
    CPUOperationStatus SetGDT(uint32_t addr, uint32_t size);

    CPUOperationStatus GetIDT(uint32_t *addr, uint32_t *size);
    CPUOperationStatus SetIDT(uint32_t addr, uint32_t size);

protected:
    CPUOperationStatus InjectInterrupt(uint8_t vector);
    bool CanInjectInterrupt();
    void RequestInterruptWindow();

private:
    Kvm *m_kvm;
    KvmVM *m_vm;
    KvmVCPU *m_vcpu;

    bool m_regsDirty;
    bool m_fpuRegsDirty;
    bool m_regsChanged;
    bool m_fpuRegsChanged;

    struct kvm_regs m_regs;
    struct kvm_sregs m_sregs;
    struct kvm_fpu m_fpuRegs;

    void UpdateRegisters();
    CPUStatus HandleExecResult(KvmVCPUStatus status);

    CPUOperationStatus HandleIO(uint8_t direction, uint16_t port, uint8_t size, uint32_t count, uint64_t dataOffset);
    CPUOperationStatus HandleMMIO(uint32_t physAddress, uint32_t *data, uint8_t size, uint8_t isWrite);

    CPUOperationStatus RefreshRegisters(bool refreshFPU);

    int LoadSegmentSelector(uint16_t selector, struct kvm_segment* segment);

};

}