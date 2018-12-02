#pragma once

#include "haxm/haxm.h"
#include "openxbox/cpu.h"
#include "openxbox/bitmap.h"

#include <mutex>
#include <queue>

namespace openxbox {
namespace cpu {

/*!
 * Intel HAXM CPU implementation.
 *
 * This is the implementation of the CPU interface using
 * [Intel HAXM](https://software.intel.com/en-us/articles/intel-hardware-accelerated-execution-manager-intel-haxm).
 */
class HaxmCpu : public Cpu {
public:
    HaxmCpu();
    ~HaxmCpu();

    CPUInitStatus InitializeImpl();

    CPUStatus RunImpl();
    CPUStatus StepImpl();
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

    CPUOperationStatus EnableSoftwareBreakpoints(bool enable) override;
    CPUOperationStatus SetHardwareBreakpoints(HardwareBreakpoints breakpoints) override;
    CPUOperationStatus ClearHardwareBreakpoints() override;
    CPUOperationStatus GetBreakpointAddress(uint32_t *address) override;

protected:
    CPUOperationStatus InjectInterrupt(uint8_t vector);
    bool CanInjectInterrupt();
    void RequestInterruptWindow();

private:
    Haxm *m_haxm;
    HaxmVM *m_vm;
    HaxmVCPU *m_vcpu;

    vcpu_state_t m_regs;
    fx_layout m_fpuRegs;
    bool m_regsDirty;        // set to true on VM exit to indicate that general registers need to be refreshed
    bool m_fpuRegsDirty;     // set to true on VM exit to indicate that floating point registers need to be refreshed
    bool m_regsChanged;      // set to true when general registers are modified by the host
    bool m_fpuRegsChanged;   // set to true when floating point registers are modified by the host

    void UpdateRegisters();
    CPUStatus HandleExecResult(HaxmVCPUStatus status);

    CPUStatus HandleIO(uint8_t df, uint16_t port, uint8_t direction, uint16_t size, uint16_t count, uint8_t *buffer);
    CPUStatus HandleFastMMIO(struct hax_fastmmio *info);

    CPUOperationStatus RefreshRegisters(bool refreshFPU);

    int LoadSegmentSelector(uint16_t selector, segment_desc_t *segment);
};

}
}
