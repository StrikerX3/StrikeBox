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
    PRAMDAC(const NV2A& nv2a) : NV2AEngine("PRAMDAC", 0x680000, 0x1000, nv2a) {}

    uint32_t Read(const uint32_t addr, const uint8_t size) override;
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
