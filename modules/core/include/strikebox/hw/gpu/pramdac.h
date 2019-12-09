// StrikeBox NV2A PRAMDAC (RAMDAC, video overlay, cursor, and PLL control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PRAMDAC engine registers occupy the range 0x680000..0x680FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A RAMDAC, video overlay, cursor, and PLL control engine (PRAMDAC)
class PRAMDAC : public NV2AEngine {
public:
    PRAMDAC(NV2A& nv2a) : NV2AEngine("PRAMDAC", 0x680000, 0x1000, nv2a) {
        Reset();
    }

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

private:
    uint32_t m_coreClockCoeff;
    uint32_t m_memoryClockCoeff;
    uint32_t m_videoClockCoeff;

    uint32_t m_mem[0x1000 / 4]; // for all other reads/writes
};

// RAMDAC registers
// [https://envytools.readthedocs.io/en/latest/hw/display/nv3/pramdac.html#id3]
const uint32_t Reg_RAMDAC_NVPLL = 0x500;  // Core PLL clock
const uint32_t Reg_RAMDAC_MPLL  = 0x504;  // Memory PLL clock
const uint32_t Reg_RAMDAC_VPLL  = 0x508;  // Video PLL clocks

}
