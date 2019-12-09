// StrikeBox NV2A PRMA (Real mode BAR access) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PRMA provides access to BAR registers through real mode.
//
// PRMA engine registers occupy the range 0x007000..0x007FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A real mode BAR access (PRMA)
class PRMA : public NV2AEngine {
public:
    PRMA(NV2A& nv2a) : NV2AEngine("PRMA", 0x007000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
