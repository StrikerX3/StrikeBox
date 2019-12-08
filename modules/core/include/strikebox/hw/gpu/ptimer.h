// StrikeBox NV2A PTIMER (Time measurement and time-based alarms) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html]
// "PTIMER is a small functional unit used to measure time by the card.
//  It has a 56-bit tick counter connected to a programmable clock source.
//  The current value of this counter is used for timestamping by many other units on the GPU.
//  Two such timestamps can be substracted to get the wall time elapsed between their creation and measure eg. command execution time.
//  Also, it's possible to set up an interrupt that will be triggered when the low 27 bits of the counter reach a specified value."
//
// PTIMER engine registers occupy the range 0x009000..0x009FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A time measurement and time-based alarms (PTIMER)
class PTIMER : public NV2AEngine {
public:
    PTIMER(const NV2A& nv2a) : NV2AEngine("PTIMER", 0x009000, 0x1000, nv2a) {}

    uint32_t Read(const uint32_t addr, const uint8_t size) override;
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size) override;
};

}
