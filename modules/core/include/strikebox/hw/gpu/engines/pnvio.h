// StrikeBox NV2A PNVIO (VGA sequencer and graph controller registers) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PNVIO engine registers occupy the range 0x0C0000..0x0C0FFF.
#pragma once

#include "../engine.h"

namespace strikebox::nv2a {

// NV2A VGA sequencer and graph controller registers (PNVIO)
class PNVIO : public NV2AEngine {
public:
    PNVIO(NV2A& nv2a) : NV2AEngine("PNVIO", 0x0C0000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
