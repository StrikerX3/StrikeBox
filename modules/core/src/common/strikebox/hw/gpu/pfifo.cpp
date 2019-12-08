// StrikeBox NV2A PFIFO (MMIO and DMA FIFO submission to PGRAPH) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/pfifo.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

uint32_t PFIFO::Read(const uint32_t addr, const uint8_t size) {
    log_spew("[NV2A] PFIFO::Read:   Unimplemented read!   address = 0x%x,  size = %u\n", addr, size);
    return 0;
}

void PFIFO::Write(const uint32_t addr, const uint32_t value, const uint8_t size) {
    log_spew("[NV2A] PFIFO::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x,  size = %u\n", addr, value, size);
}

}
