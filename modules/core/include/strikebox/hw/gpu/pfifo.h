// StrikeBox NV2A PFIFO (MMIO and DMA FIFO submission to PGRAPH) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/fifo/intro.html]
// "Commands to most of the engines are sent through a special engine called PFIFO.
//  PFIFO maintains multiple fully independent command queues, known as "channels" or "FIFO"s.
//  Each channel is controlled through a "channel control area", which is a region of MMIO.
//  PFIFO intercepts all accesses to that area and acts upon them."
//
// PFIFO engine registers occupy the range 0x002000..0x003FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// NV2A MMIO and DMA FIFO submission to PGRAPH engine (PFIFO)
class PFIFO : public NV2AEngine {
public:
    PFIFO(NV2A& nv2a) : NV2AEngine("PFIFO", 0x002000, 0x2000, nv2a) {
        Reset();
    }

    void SetEnabled(bool enabled);

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

private:
    bool m_enabled = false;
};

}
