#pragma once

#include "haxm/haxm.h"
#include "openxbox/cpu.h"
#include "openxbox/bitmap.h"

#include <mutex>
#include <queue>

namespace openxbox {

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

    int InitializeImpl();

	int RunImpl();
	int StepImpl(uint64_t num_instructions);
	InterruptResult InterruptImpl(uint8_t vector);

	int MemMapSubregion(MemoryRegion *subregion);
	int MemRead(uint32_t addr, uint32_t size, void *value);
    int MemWrite(uint32_t addr, uint32_t size, void *value);

	int RegRead(enum CpuReg reg, uint32_t *value);
    int RegWrite(enum CpuReg reg, uint32_t value);

	int GetGDT(uint32_t *addr, uint32_t *size);
	int SetGDT(uint32_t addr, uint32_t size);

	int GetIDT(uint32_t *addr, uint32_t *size);
	int SetIDT(uint32_t addr, uint32_t size);

protected:
    int InjectInterrupt(uint8_t vector);

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
	
	int HandleIO(uint8_t df, uint16_t port, uint8_t direction, uint16_t size, uint16_t count, uint8_t *buffer);
	int HandleFastMMIO(struct hax_fastmmio *info);

	int RefreshRegisters(bool refreshFPU);

	int LoadSegmentSelector(uint16_t selector, segment_desc_t *segment);
};

}
