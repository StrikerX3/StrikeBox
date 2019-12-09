// StrikeBox NV2A PMC (Master control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html]
// "PMC is the "master control" engine of the card. Its purpose is to provide card identication,
//  manage enable/disable bits of other engines, and handle top-level interrupt routing."
//
// PMC engine registers occupy the range 0x000000..0x000FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A master control engine (PMC)
class PMC : public NV2AEngine {
public:
    PMC(NV2A& nv2a) : NV2AEngine("PMC", 0x000000, 0x1000, nv2a) {
        Reset();
    }

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

// PMC registers
// [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html#id3]
const uint32_t Reg_PMC_ID               = 0x000;  // Card identification
const uint32_t Reg_PMC_INTR_HOST        = 0x100;  // Interrupt status - host
const uint32_t Reg_PMC_INTR_ENABLE_HOST = 0x140;  // Interrupt enable - host

}
