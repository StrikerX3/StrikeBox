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

    int InitializeImpl();

    int RunImpl();
    int StepImpl(uint64_t num_instructions);
    InterruptResult InterruptImpl(uint8_t vector);

    int MemMapSubregion(MemoryRegion *subregion);

    int RegRead(enum CpuReg reg, uint32_t *value);
    int RegWrite(enum CpuReg reg, uint32_t value);

    int GetGDT(uint32_t *addr, uint32_t *size);
    int SetGDT(uint32_t addr, uint32_t size);

    int GetIDT(uint32_t *addr, uint32_t *size);
    int SetIDT(uint32_t addr, uint32_t size);

protected:
	int InjectInterrupt(uint8_t vector);
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

    int HandleIO(uint8_t direction, uint16_t port, uint8_t size, uint32_t count, uint64_t dataOffset);
    int HandleMMIO(uint32_t physAddress, uint32_t *data, uint8_t size, uint8_t isWrite);

    int RefreshRegisters(bool refreshFPU);

    int LoadSegmentSelector(uint16_t selector, struct kvm_segment* segment);

};

}