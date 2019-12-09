// StrikeBox NV2A PRAMDAC (RAMDAC, video overlay, cursor, and PLL control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/pramdac.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PRAMDAC::Reset() {
    // NV2A clocks:
    // crystal = 16.6 MHz
    //    core = 233 MHz
    //  memory = 200 MHz
    //   video = 25.160 MHz
    //
    // The clocks are calculated as such: (Crystal frequency * N) / (1 << P) / M.
    m_coreClockCoeff = ClockCoefficients{ 1, 28, 1 }.u32;
    m_memoryClockCoeff = ClockCoefficients{ 1, 24, 1 }.u32;
    m_videoClockCoeff = ClockCoefficients{ 3, 157, 13 }.u32;

    std::fill(std::begin(m_mem), std::end(m_mem), 0);
}

uint32_t PRAMDAC::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_RAMDAC_NVPLL: return m_coreClockCoeff;
    case Reg_RAMDAC_MPLL: return m_memoryClockCoeff;
    case Reg_RAMDAC_VPLL: return m_videoClockCoeff;
    default:
        log_spew("[NV2A] PRAMDAC::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return m_mem[addr >> 2];
    }
}

void PRAMDAC::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_RAMDAC_NVPLL: m_coreClockCoeff = value; break;
    case Reg_RAMDAC_MPLL: m_memoryClockCoeff = value; break;
    case Reg_RAMDAC_VPLL: m_videoClockCoeff = value; break;
    default:
        log_spew("[NV2A] PRAMDAC::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        m_mem[addr >> 2] = value;
        break;
    }
}

}
