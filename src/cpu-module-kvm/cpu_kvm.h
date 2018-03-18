#pragma once

#include "openxbox/cpu.h"
#include "kvm/kvm.h"

#include <linux/kvm.h>
#include <vector>
#include <queue>
#include <mutex>
#include <openxbox/bitmap.h>

namespace openxbox {

#ifdef _DEBUG
    static const uint8_t kInterruptHandlerMaxCredits = 25;   // Maximum amount of credits available to handle interrupts
    static const uint8_t kInterruptHandlerCost = 5;          // Credits spent when an interrupt is handled
    static const uint8_t kInterruptHandlerIncrement = 1;     // Credits recovered when CPU emulation starts
#else
    static const uint8_t kInterruptHandlerMaxCredits = 200;  // Maximum amount of credits available to handle interrupts
    static const uint8_t kInterruptHandlerCost = 2;          // Credits spent when an interrupt is handled
    static const uint8_t kInterruptHandlerIncrement = 1;     // Credits recovered when CPU emulation starts
#endif

class KvmCpu : public Cpu {

public:
    KvmCpu();
    ~KvmCpu();

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

    int HandleIO(uint8_t direction, uint16_t port, uint8_t size, uint32_t count, uint64_t dataOffset);
    int HandleMMIO(uint32_t physAddress, uint32_t *data, uint8_t size, uint8_t isWrite);

    void InjectPendingInterrupt();

    Kvm *m_kvm;
    KvmVM *m_vm;
    KvmVCPU *m_vcpu;

    std::mutex m_interruptMutex;
    std::mutex m_pendingInterruptsMutex;
    std::queue<uint8_t> m_pendingInterrupts;
    Bitmap64 m_pendingInterruptsBitmap;
    uint8_t m_interruptHandlerCredits;


};

}