// StrikeBox NV2A PBUS (Bus control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

uint32_t NV2AEngine::ReadUnaligned(const uint32_t addr, const uint8_t size) {
    // Unaligned read fits in one register
    if ((addr & ~3) == ((addr + size - 1) & ~3)) {
        log_warning("NV2AEngine::Read:   Unaligned read!   address = 0x%x,  size = %u\n", addr + m_offset, size);
        uint8_t offset = (addr & 3) * 8;
        uint32_t mask = 0xFFFFFFFF >> (32 - size * 8);
        return (Read(addr & ~3) >> offset) & mask;
    }

    // Unaligned read spans two registers, corresponding to one of these cases:
    // - 16-bit read from offset 3
    // - 32-bit read from offsets 1, 2 or 3
    // FIXME: how does the real hardware behave in this case?
    log_warning("NV2AEngine::Read:   Severely unaligned read!   address = 0x%x,  size = %u\n", addr + m_offset, size);
    
    uint32_t value1 = Read(addr & ~3);
    uint32_t value2 = Read((addr & ~3) + 4);

    uint8_t offset = (addr & 3) * 8;
    uint32_t mask = 0xFFFFFFFF >> (32 - size * 8);
    return ((value1 >> offset) | (value2 << (32 - offset))) & mask;
}

void NV2AEngine::WriteUnaligned(const uint32_t addr, const uint32_t value, const uint8_t size) {
    log_warning("NV2AEngine::Write:  Unaligned write!   address = 0x%x,  size = %u\n", addr + m_offset, size);
    // FIXME: how does the real hardware behave in this case?
    // This is almost certainly wrong.
    Write(addr & ~3, value);
}

}
