#ifndef CPU_UNICORN_H
#define CPU_UNICORN_H

#include <unicorn/unicorn.h>
#include "cpu.h"

/*!
 * Unicorn CPU Implementation
 * 
 * This is the implementation of the CPU interface using the [Unicorn CPU
 * emulator engine](http://www.unicorn-engine.org/).
 */
class UnicornCpu : public Cpu {
protected:
    uc_engine *m_uc;

public:
    UnicornCpu();
    ~UnicornCpu();
    int Initialize();
    int MemRead(uint32_t addr, size_t size, void *value);
    int MemWrite(uint32_t addr, size_t size, void *value);
    int RegRead(enum CpuReg reg, uint32_t *value);
    int RegWrite(enum CpuReg reg, uint32_t value);
    int Run(uint64_t time_limit_us);
    int MemMap(MemoryRegion *mem);
};

#endif
