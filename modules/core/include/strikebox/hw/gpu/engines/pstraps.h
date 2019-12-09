// StrikeBox NV2A PSTRAPS (Straps readout) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/io/pstraps.html]
// "The nvidia GPU chips are used in multiple cards from multiple manufacturers.
//  Thus, the a single GPU can end up in many different configurations, with varying memory amount, memory type, bus type,
//  TV norm, crystal frequency, and many other parameters. Since the GPU often needs to know what configuration it is used in,
//  a "straps" mechanism was invented to tell it this information.
//  On the first few cycles after reset, the memory bus pins are sampled. Since nothing else is driving them at that point,
//  their logic state is decided by the pull-up or pull-down resistors placed by board manufacturer.
//  The value this read is used as the "straps" value and is used to configure many aspects of GPU operation.
//  Some of the straps are not used by the GPU itself, but are intended for use by the BIOS or the driver."
//
// PSTRAPS engine registers occupy the range 0x101000..0x101FFF.
#pragma once

#include "../engine.h"

namespace strikebox::nv2a {

// NV2A straps readout engine (PSTRAPS)
class PSTRAPS : public NV2AEngine {
public:
    PSTRAPS(NV2A& nv2a) : NV2AEngine("PSTRAPS", 0x101000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
