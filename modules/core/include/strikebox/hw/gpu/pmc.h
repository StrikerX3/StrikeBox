// StrikeBox NV2A PMC (Master control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html]
// "PMC is the "master control" engine of the card. Its purpose is to provide card identication,
//  manage enable/disable bits of other engines, and handle top-level interrupt routing."
//
// PMC engine registers occupy the range 0x000000..0x000FFF.
#pragma once

#include "engine.h"

namespace strikebox::nv2a {

// PMC registers
// [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html#mmio-register-list]
const uint32_t Reg_PMC_ID                 = 0x000;                // [R ] Card identification
const uint32_t Reg_PMC_INTR_HOST          = 0x100;                // [RW] Interrupt status - host  [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html#interrupts]
/**/const uint32_t Val_PMC_INTR_HOST_PFIFO    = (1 << 8);         //  bit  8: PFIFO
/**/const uint32_t Val_PMC_INTR_HOST_PGRAPH   = (1 << 12);        //  bit 12: PGRAPH
/**/const uint32_t Val_PMC_INTR_HOST_PVIDEO   = (1 << 16);        //  bit 16: PVIDEO
/**/const uint32_t Val_PMC_INTR_HOST_PTIMER   = (1 << 20);        //  bit 20: PTIMER
/**/const uint32_t Val_PMC_INTR_HOST_PCRTC    = (1 << 24);        //  bit 24: PCRTC
/**/const uint32_t Val_PMC_INTR_HOST_PBUS     = (1 << 28);        //  bit 28: PBUS
/**/const uint32_t Val_PMC_INTR_HOST_SOFTWARE = (1 << 31);        //  bit 31: software
const uint32_t Reg_PMC_INTR_ENABLE_HOST   = 0x140;                // [RW] Interrupt enable - host  [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html#interrupts]
/**/const uint32_t Val_PMC_INTR_ENABLE_HOST_HARDWARE = (1 << 0);  // bit 0: hardware interrupt enable
/**/const uint32_t Val_PMC_INTR_ENABLE_HOST_SOFTWARE = (1 << 1);  // bit 1: software interrupt enable
const uint32_t Reg_PMC_ENABLE             = 0x200;                // [RW] Engine enables  [https://envytools.readthedocs.io/en/latest/hw/bus/pmc.html#engine-enables]
/**/const uint32_t Val_PMC_ENABLE_PFIFO       = (1 << 8);         //  bit  8: PFIFO
/**/const uint32_t Val_PMC_ENABLE_PGRAPH      = (1 << 12);        //  bit 12: PGRAPH
/**/const uint32_t Val_PMC_ENABLE_PTIMER      = (1 << 16);        //  bit 16: PTIMER
/**/const uint32_t Val_PMC_ENABLE_PFB         = (1 << 20);        //  bit 20: PFB
/**/const uint32_t Val_PMC_ENABLE_PCRTC       = (1 << 24);        //  bit 24: PCRTC
/**/const uint32_t Val_PMC_ENABLE_PVIDEO      = (1 << 28);        //  bit 28: PVIDEO

// ----------------------------------------------------------------------------

// NV2A master control engine (PMC)
class PMC : public NV2AEngine {
public:
    PMC(NV2A& nv2a) : NV2AEngine("PMC", 0x000000, 0x1000, nv2a) {}

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

    void UpdateIRQ();

private:
    uint32_t m_interruptLevels;    // INTR_HOST
    uint32_t m_enabledInterrupts;  // INTR_ENABLE_HOST
    uint32_t m_enabledEngines;     // ENABLE

    void SetEngineEnables(uint32_t enables);
};

}
