// StrikeBox NV2A PFB (Memory interface) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PFB engine registers occupy the range 0x100000..0x100FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A memory interface engine (PFB)
class PFB : public NV2AEngine {
public:
    PFB(NV2A& nv2a) : NV2AEngine("PFB", 0x100000, 0x1000, nv2a) {
        Reset();
    }

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
