// StrikeBox NV2A PCRTC (CRTC controls) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pcrtc.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PCRTC::SetEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (enabled) {
            // TODO: start
        }
        else {
            Reset();
        }
    }
}

void PCRTC::Reset() {
    m_enabled = false;
    m_interruptLevels = 0;
    m_enabledInterrupts = 0;
}

uint32_t PCRTC::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PCRTC_INTR: return m_interruptLevels;
    case Reg_PCRTC_INTR_ENABLE: return m_enabledInterrupts;
    default:
        log_spew("[NV2A] PCRTC::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PCRTC::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PCRTC_INTR:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PCRTC_INTR_ENABLE:
        m_enabledInterrupts = value;
        m_nv2a.UpdateIRQ();
        break;
    default:
        log_spew("[NV2A] PCRTC::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        break;
    }
}

}
