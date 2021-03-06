// StrikeBox NV2A PRMCIO (VGA CRTC and attribute controller registers) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PRMCIO engine registers occupy the range 0x601000..0x601FFF.
#pragma once

#include "../engine.h"

namespace strikebox::nv2a {

// NV2A VGA CRTC and attribute controller registers engine (PRMCIO)
class PRMCIO : public NV2AEngine {
public:
    PRMCIO(NV2A& nv2a) : NV2AEngine("PRMCIO", 0x601000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
    uint32_t ReadUnaligned(const uint32_t addr, const uint8_t size) override;
    void WriteUnaligned(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
