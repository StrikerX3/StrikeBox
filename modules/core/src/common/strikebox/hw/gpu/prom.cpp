// StrikeBox NV2A PROM (ROM access window) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/prom.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

uint32_t PROM::Read(const uint32_t addr, const uint8_t size) {
    log_spew("[NV2A] PROM::Read:   Unimplemented read!   address = 0x%x,  size = %u\n", addr, size);
    return 0;
}

void PROM::Write(const uint32_t addr, const uint32_t value, const uint8_t size) {
    log_spew("[NV2A] PROM::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x,  size = %u\n", addr, value, size);
}

}
