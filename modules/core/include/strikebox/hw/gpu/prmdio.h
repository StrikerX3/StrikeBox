// StrikeBox NV2A PRMDIO (VGA DAC registers) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PRMDIO engine registers occupy the range 0x681000..0x681FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A VGA DAC registers engine (PRMDIO)
class PRMDIO : public NV2AEngine {
public:
    PRMDIO(NV2A& nv2a) : NV2AEngine("PRMDIO", 0x681000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
