#pragma once

#include "whvp/whvp.h"
#include "vixen/cpu.h"

namespace vixen {
namespace cpu {

/*!
 * Windows Hypervisor Platform CPU implementation.
 *
 * This is the implementation of the CPU interface using
 * [Windows Hypervisor Platform](https://docs.microsoft.com/en-us/virtualization/api/).
 */
class WhvpCpu : public Cpu {
public:
    WhvpCpu();
    ~WhvpCpu();

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
    WinHvPlatform *m_whvp;
    WHvPartition *m_partition;
    WHvVCPU *m_vcpu;

    static HRESULT IoPortCallback(PVOID context, WHV_EMULATOR_IO_ACCESS_INFO *io);
    static HRESULT MemoryCallback(PVOID context, WHV_EMULATOR_MEMORY_ACCESS_INFO *mem);
};

}
}
