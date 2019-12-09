// StrikeBox NV2A PCRTC (CRTC controls) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/pcrtc.h"

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
}

uint32_t PCRTC::Read(const uint32_t addr) {
    log_spew("[NV2A] PCRTC::Read:   Unimplemented read!   address = 0x%x\n", addr);
    return 0;
}

void PCRTC::Write(const uint32_t addr, const uint32_t value) {
    log_spew("[NV2A] PCRTC::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
}

}
