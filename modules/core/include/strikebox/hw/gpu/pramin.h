// StrikeBox NV2A PRAMIN (RAMIN access) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PRAMIN contains a memory area reserved by the kernel used to describe GPU objects.
//
// PRAMIN engine registers occupy the range 0x700000..0x7FFFFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A RAMIN access engine (PRAMIN)
class PRAMIN : public NV2AEngine {
public:
    PRAMIN(NV2A& nv2a) : NV2AEngine("PRAMIN", 0x700000, 0x100000, nv2a) {
        Reset();
    }

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
