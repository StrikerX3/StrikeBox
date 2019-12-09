// StrikeBox NV2A PGRAPH (2D/3D graphics engine) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pgraph.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PGRAPH::SetEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (enabled) {
            // TODO: start threads
        }
        else {
            Reset();
        }
    }
}

void PGRAPH::Reset() {
    m_enabled = false;
    m_interruptLevels = 0;
    m_enabledInterrupts = 0;
    // TODO: stop threads
}

uint32_t PGRAPH::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PGRAPH_INTR: return m_interruptLevels;
    case Reg_PGRAPH_INTR_ENABLE: return m_enabledInterrupts;
    default:
        log_spew("[NV2A] PGRAPH::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PGRAPH::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PGRAPH_INTR:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PGRAPH_INTR_ENABLE:
        m_enabledInterrupts = value;
        m_nv2a.UpdateIRQ();
        break;
    default:
        log_spew("[NV2A] PGRAPH::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        break;
    }
}

}
