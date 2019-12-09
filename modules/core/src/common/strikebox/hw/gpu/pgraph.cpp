// StrikeBox NV2A PGRAPH (2D/3D graphics engine) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/pgraph.h"

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
    // TODO: stop threads
}

uint32_t PGRAPH::Read(const uint32_t addr) {
    log_spew("[NV2A] PGRAPH::Read:   Unimplemented read!   address = 0x%x\n", addr);
    return 0;
}

void PGRAPH::Write(const uint32_t addr, const uint32_t value) {
    log_spew("[NV2A] PGRAPH::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
}

}
