// StrikeBox NV2A PBUS (Bus control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/bus/pbus.html]
// "PBUS is present on all nvidia cards.
//  In theory, it deals with "bus control". In practice, it accumulates all sort of junk nobody bothered to create a special area for.
//  It is unaffected by any PMC.ENABLE bits."
//
// PBUS engine registers occupy the range 0x001000..0x001FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A bus control engine (PBUS)
class PBUS : public NV2AEngine {
public:
    PBUS(const NV2A& nv2a) : NV2AEngine("PBUS", 0x001000, 0x1000, nv2a) {}

    uint32_t Read(const uint32_t addr, const uint8_t size) override;
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
