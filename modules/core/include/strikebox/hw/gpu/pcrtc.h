// StrikeBox NV2A PCRTC (CRTC controls) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PCRTC engine registers occupy the range 0x600000..0x600FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// PCRTC registers
// [https://envytools.readthedocs.io/en/latest/hw/display/nv3/pcrtc.html#mmio-registers]
const uint32_t Reg_PCRTC_INTR = 0x100;                // [RW] Interrupt status
const uint32_t Reg_PCRTC_INTR_ENABLE = 0x140;         // [RW] Interrupt enable
/**/const uint32_t Val_PCRTC_INTR_VBLANK = (1 << 0);  //  bit  0: Vertical blank

// ----------------------------------------------------------------------------

// NV2A CRTC controls engine (PCRTC)
class PCRTC : public NV2AEngine {
public:
    PCRTC(NV2A& nv2a) : NV2AEngine("PCRTC", 0x600000, 0x1000, nv2a) {}

    void SetEnabled(bool enabled);

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

    bool GetInterruptState() { return m_interruptLevels & m_enabledInterrupts; }

private:
    bool m_enabled = false;

    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;
};

}
