// StrikeBox NV2A PVIDEO (Video overlay) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PVIDEO engine registers occupy the range 0x008000..0x008FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A video overlay engine (PVIDEO)
class PVIDEO : public NV2AEngine {
public:
    PVIDEO(const NV2A& nv2a) : NV2AEngine("PVIDEO", 0x008000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr, const uint8_t size) override;
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
