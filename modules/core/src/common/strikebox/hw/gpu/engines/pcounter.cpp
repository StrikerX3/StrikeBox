// StrikeBox NV2A PCOUNTER (Performance monitoring counters) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pcounter.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PCOUNTER::Reset() {
}

uint32_t PCOUNTER::Read(const uint32_t addr) {
    log_spew("[NV2A] PCOUNTER::Read:   Unimplemented read!   address = 0x%x\n", addr);
    return 0;
}

void PCOUNTER::Write(const uint32_t addr, const uint32_t value) {
    log_spew("[NV2A] PCOUNTER::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
}

}
