#pragma once

#include "vixen/cpu.h"
#include "vixen/log.h"

namespace vixen {

using namespace vixen::cpu;

/*!
 * Print the CPU registers
 */
void DumpCPURegisters(Cpu *cpu);

/*!
 * Dump CPU stack
 */
void DumpCPUStack(Cpu *cpu, int32_t offsetStart = -0x20, int32_t offsetEnd = 0x10);

/*!
 * Dump memory
 */
void DumpCPUMemory(Cpu *cpu, uint32_t address, uint32_t size, bool physical);

/*!
 * Disassemble memory region
 */
void DumpCPUDisassembly(Cpu *cpu, uint32_t address, uint32_t size, bool physical);

}
