// StrikeBox NV2A PCOUNTER (Performance monitoring counters) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/pcounter/intro.html]
// "PCOUNTER is the card units that contains performance monitoring counters.
//  [...]
//  PCOUNTER is actually made of several identical hardware counter units, one for each so-called domain.
//  Each PCOUNTER domain can potentially run on a different source clock, allowing one to monitor events in various clock domains.
//  The PCOUNTER domains are mostly independent, but there's some limitted communication and shared circuitry among them."
//
// PCOUNTER engine registers occupy the range 0x00A000..0x00AFFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A performance monitoring counters engine (PCOUNTER)
class PCOUNTER : public NV2AEngine {
public:
    PCOUNTER(NV2A& nv2a) : NV2AEngine("PCOUNTER", 0x00A000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;
};

}
