// StrikeBox NV2A PNVIO (VGA sequencer and graph controller registers) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pnvio.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PNVIO::Reset() {
}

uint32_t PNVIO::Read(const uint32_t addr) {
    log_spew("[NV2A] PNVIO::Read:   Unimplemented read!   address = 0x%x\n", addr);
    return 0;
}

void PNVIO::Write(const uint32_t addr, const uint32_t value) {
    log_spew("[NV2A] PNVIO::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
}

uint32_t PNVIO::ReadUnaligned(const uint32_t addr, const uint8_t size) {
    log_spew("[NV2A] PNVIO::ReadUnaligned:   Unimplemented unaligned read!   address = 0x%x,  size = %u\n", addr, size);
    return 0;
}

void PNVIO::WriteUnaligned(const uint32_t addr, const uint32_t value, const uint8_t size) {
    log_spew("[NV2A] PNVIO::WriteUnaligned:  Unimplemented unaligned write!   address = 0x%x,  value = 0x%x,  size = %u\n", addr, value, size);
}

}
