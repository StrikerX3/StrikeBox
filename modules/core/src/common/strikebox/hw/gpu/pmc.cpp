// StrikeBox NV2A PMC (Master control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/pmc.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PMC::Reset() {
    m_enabledEngines = 0;
    m_enabledInterrupts = 0;
    m_interruptLevels = 0;
    UpdateIRQ();
}

uint32_t PMC::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PMC_ID:
        return (0x2A << 20)   // GPU ID: NV2A
            | (0 << 16)       // Device ID: 0
            | (0xA2);         // Revision: A02
    case Reg_PMC_INTR_HOST: return m_interruptLevels;
    case Reg_PMC_INTR_ENABLE_HOST: return m_enabledInterrupts;
    case Reg_PMC_ENABLE: return m_enabledEngines;
    default:
        log_spew("[NV2A] PMC::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PMC::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PMC_ID:
        // Discard writes to read-only registers
        break;
    case Reg_PMC_INTR_HOST:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        UpdateIRQ();
        break;
    case Reg_PMC_INTR_ENABLE_HOST:
        m_enabledInterrupts = value;
        UpdateIRQ();
        break;
    case Reg_PMC_ENABLE:
        SetEngineEnables(value);
        break;
    default:
        log_spew("[NV2A] PMC::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        break;
    }
}

static inline void SetOrClearBit(uint32_t& bitset, const uint32_t bit, const bool set) {
    if (set) {
        bitset |= bit;
    }
    else {
        bitset &= ~bit;
    }
}

void PMC::UpdateIRQ() {
    SetOrClearBit(m_interruptLevels, Val_PMC_INTR_HOST_PFIFO, m_nv2a.pfifo->GetInterruptState());
    SetOrClearBit(m_interruptLevels, Val_PMC_INTR_HOST_PGRAPH, m_nv2a.pgraph->GetInterruptState());
    SetOrClearBit(m_interruptLevels, Val_PMC_INTR_HOST_PVIDEO, m_nv2a.pvideo->GetInterruptState());
    SetOrClearBit(m_interruptLevels, Val_PMC_INTR_HOST_PTIMER, m_nv2a.ptimer->GetInterruptState());
    SetOrClearBit(m_interruptLevels, Val_PMC_INTR_HOST_PCRTC, m_nv2a.pcrtc->GetInterruptState());
    SetOrClearBit(m_interruptLevels, Val_PMC_INTR_HOST_PBUS, m_nv2a.pbus->GetInterruptState());

    bool level = ((m_interruptLevels & ~Val_PMC_INTR_HOST_SOFTWARE) && (m_enabledInterrupts & Val_PMC_INTR_ENABLE_HOST_HARDWARE))
        || ((m_interruptLevels & Val_PMC_INTR_HOST_SOFTWARE) && (m_enabledInterrupts & Val_PMC_INTR_ENABLE_HOST_SOFTWARE));
    
    m_nv2a.handleIRQ(level);
}

void PMC::SetEngineEnables(uint32_t enables) {
    m_enabledEngines = enables;
    m_nv2a.pfifo->SetEnabled(enables & Val_PMC_ENABLE_PFIFO);
    m_nv2a.pgraph->SetEnabled(enables & Val_PMC_ENABLE_PGRAPH);
    m_nv2a.ptimer->SetEnabled(enables & Val_PMC_ENABLE_PTIMER);
    m_nv2a.pfb->SetEnabled(enables & Val_PMC_ENABLE_PFB);
    m_nv2a.pcrtc->SetEnabled(enables & Val_PMC_ENABLE_PCRTC);
    m_nv2a.pvideo->SetEnabled(enables & Val_PMC_ENABLE_PVIDEO);
}

}
