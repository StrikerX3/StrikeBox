// StrikeBox NV2A PMC (Master control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/pmc.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PMC::Reset() {
}

uint32_t PMC::Read(const uint32_t addr, const uint8_t size) {
    log_spew("[NV2A] PMC::Read:   Unimplemented read!   address = 0x%x,  size = %u\n", addr, size);
    return 0;
}

void PMC::Write(const uint32_t addr, const uint32_t value, const uint8_t size) {
    log_spew("[NV2A] PMC::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x,  size = %u\n", addr, value, size);
}

}
