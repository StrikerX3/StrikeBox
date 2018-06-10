#pragma once

#include "whvp/whvp.h"
#include "openxbox/cpu.h"

namespace openxbox {

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
	WinHvPlatform *m_whvp;
	WHvPartition *m_partition;
	WHvVCPU *m_vcpu;

    static HRESULT IoPortCallback(PVOID context, WHV_EMULATOR_IO_ACCESS_INFO *io);
    static HRESULT MemoryCallback(PVOID context, WHV_EMULATOR_MEMORY_ACCESS_INFO *mem);
};

}
