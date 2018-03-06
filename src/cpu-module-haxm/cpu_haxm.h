#pragma once

#include "haxm/haxm.h"
#include "openxbox/cpu.h"
#include "openxbox/bitmap.h"

#include <mutex>
#include <queue>

namespace openxbox {

// These constants control how often the CPU can handle interrupts, while
// giving some affordance to handle short bursts.
// Without this, the emulator might get stuck processing interrupts alone.
#ifdef _DEBUG
    static const uint8_t kInterruptHandlerMaxCredits = 25;   // Maximum amount of credits available to handle interrupts
    static const uint8_t kInterruptHandlerCost = 5;          // Credits spent when an interrupt is handled
    static const uint8_t kInterruptHandlerIncrement = 1;     // Credits recovered when CPU emulation starts
#else
    static const uint8_t kInterruptHandlerMaxCredits = 200;  // Maximum amount of credits available to handle interrupts
    static const uint8_t kInterruptHandlerCost = 2;          // Credits spent when an interrupt is handled
    static const uint8_t kInterruptHandlerIncrement = 1;     // Credits recovered when CPU emulation starts
#endif


struct PhysicalMemoryRange {
    char *data;
    uint32_t startingAddress;
    uint32_t endingAddress;
};

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

	int ReadMSR(uint32_t reg, uint64_t *value);
	int WriteMSR(uint32_t reg, uint64_t value);
	
	int InvalidateTLBEntry(uint32_t addr);
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
	
	// TODO: use an AVL tree instead of a vector to speed up lookups
	std::vector<PhysicalMemoryRange *> m_physMemMap;

    std::mutex m_interruptMutex;
    std::mutex m_pendingInterruptsMutex;
    std::queue<uint8_t> m_pendingInterrupts;
    Bitmap64 m_pendingInterruptsBitmap;  // Prevents the same interrupt from being enqueued more than once
    uint8_t m_interruptHandlerCredits;

	int HandleIO(uint8_t df, uint16_t port, uint8_t direction, uint16_t size, uint16_t count, uint8_t *buffer);
	int HandleMMIO(uint32_t physAddress, uint8_t direction);
	int HandleFastMMIO(struct hax_fastmmio *info);

	int RefreshRegisters(bool refreshFPU);

	int LoadSegmentSelector(uint16_t selector, segment_desc_t *segment);

    void InjectPendingInterrupt();
};

}
