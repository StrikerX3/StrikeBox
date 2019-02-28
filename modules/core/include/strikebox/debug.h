#pragma once

#include "virt86/virt86.hpp"

#include "strikebox/log.h"

namespace strikebox {

/*!
 * Print the CPU registers
 */
void DumpCPURegisters(virt86::VirtualProcessor& vp);

/*!
 * Dump CPU stack
 */
void DumpCPUStack(virt86::VirtualProcessor& vp, int32_t offsetStart = -0x20, int32_t offsetEnd = 0x10);

/*!
 * Dump memory
 */
void DumpCPUMemory(virt86::VirtualProcessor& vp, uint32_t address, uint32_t size, bool physical);

/*!
 * Disassemble memory region
 */
void DumpCPUDisassembly(virt86::VirtualProcessor& vp, uint32_t address, uint32_t size, bool physical);

}
