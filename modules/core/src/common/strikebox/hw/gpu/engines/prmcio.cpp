// StrikeBox NV2A PRMCIO (VGA CRTC and attribute controller registers) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/prmcio.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PRMCIO::Reset() {
}

uint32_t PRMCIO::Read(const uint32_t addr) {
    log_spew("[NV2A] PRMCIO::Read:   Unimplemented read!   address = 0x%x\n", addr);
    return 0;
}

void PRMCIO::Write(const uint32_t addr, const uint32_t value) {
    log_spew("[NV2A] PRMCIO::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
}

}
