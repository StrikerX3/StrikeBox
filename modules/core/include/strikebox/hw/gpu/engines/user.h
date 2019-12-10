// StrikeBox NV2A USER (PFIFO MMIO/DMA submission area) emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// The USER area is a portion of shared system memory where FIFO commands are submitted
// for processing by the GPU.
//
// USER engine registers occupy the range 0x800000..0x9FFFFF.
#pragma once

#include "../engine.h"

namespace strikebox::nv2a {

// NV2A PFIFO MMIO/DMA submission area (USER)
class USER : public NV2AEngine {
public:
    USER(NV2A& nv2a) : NV2AEngine("USER", 0x800000, 0x200000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
