// StrikeBox NV2A PBUS (Bus control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
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

#include "../engine.h"

namespace strikebox::nv2a {

// PBUS registers
// [https://envytools.readthedocs.io/en/latest/hw/bus/pbus.html#mmio-registers]
const uint32_t Reg_PBUS_INTR = 0x100;        // [RW] Interrupt status
const uint32_t Reg_PBUS_INTR_ENABLE = 0x140; // [RW] Interrupt enable

// ----------------------------------------------------------------------------

// NV2A bus control engine (PBUS)
class PBUS : public NV2AEngine {
public:
    PBUS(NV2A& nv2a) : NV2AEngine("PBUS", 0x001000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

    bool GetInterruptState() { return m_interruptLevels & m_enabledInterrupts; }

private:
    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;
};

}
