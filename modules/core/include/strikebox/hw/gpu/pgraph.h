// StrikeBox NV2A PGRAPH (2D/3D graphics engine) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PGRAPH is the main engine responsible for 2D/3D graphics command processing.
//
// PGRAPH engine registers occupy the range 0x400000..0x401FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A 2D/3D graphics engine (PGRAPH)
class PGRAPH : public NV2AEngine {
public:
    PGRAPH(const NV2A& nv2a) : NV2AEngine("PGRAPH", 0x400000, 0x2000, nv2a) {}

    uint32_t Read(const uint32_t addr, const uint8_t size) override;
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
