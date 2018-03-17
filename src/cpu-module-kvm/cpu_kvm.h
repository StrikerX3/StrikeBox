#pragma once

#include "openxbox/cpu.h"

#include <linux/kvm.h>
#include <vector>

namespace openxbox {

typedef struct {
    uint32_t startingAddress;
    uint32_t size;
    struct kvm_userspace_memory_region memoryRegion;
} PhysicalMemoryRange;

typedef struct {
    int dev_id;
    int dev_fd;
    struct kvm_run* kvm_run;
    int kvm_run_mmap_size;
    struct kvm_regs regs;
    struct kvm_sregs sregs;
    struct kvm_guest_debug debug;
} KvmVCpu;

typedef struct {
    int dev_fd;
    int vm_fd;
    std::vector<PhysicalMemoryRange*> physicalMemoryRanges;
    KvmVCpu vCpu;
} Kvm;

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
    Kvm *m_kvm;
};

}