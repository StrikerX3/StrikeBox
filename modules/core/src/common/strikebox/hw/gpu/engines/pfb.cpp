// StrikeBox NV2A PFB (Memory interface) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pfb.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PFB::SetEnabled(bool enabled) {
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

void PFB::Reset() {
    std::fill(std::begin(m_mem), std::end(m_mem), 0);
    m_enabled = false;
}

uint32_t PFB::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PFB_CFG0: return 3;   // The kernel asserts this value to be 3 early during initialization
    case Reg_PFB_CSTATUS: return m_nv2a.systemRAMSize;
    case Reg_PFB_WBC: return 0;
    default:
        //log_spew("[NV2A] PFB::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return m_mem[addr >> 2];
    }
}

void PFB::Write(const uint32_t addr, const uint32_t value) {
    //log_spew("[NV2A] PFB::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
    m_mem[addr >> 2] = value;
}

}
