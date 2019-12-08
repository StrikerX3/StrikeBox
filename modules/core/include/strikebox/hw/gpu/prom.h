// StrikeBox NV2A PROM (ROM access window) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PROM engine registers occupy the range 0x300000..0x31FFFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A ROM access window engine (PROM)
class PROM : public NV2AEngine {
public:
    PROM(const NV2A& nv2a) : NV2AEngine("PROM", 0x300000, 0x20000, nv2a) {}

    uint32_t Read(const uint32_t addr, const uint8_t size) override;
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
