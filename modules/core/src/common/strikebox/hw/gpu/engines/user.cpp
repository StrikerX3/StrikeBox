// StrikeBox NV2A USER (PFIFO MMIO/DMA submission area) emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/user.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

// [https://envytools.readthedocs.io/en/latest/hw/fifo/nv4-pfifo.html#space-nv4-user]
// 0x800000 + chid*0x10000 + subc*0x2000: USER[chid][subc]  (chid<32, subc<8)

// USER registers
const uint32_t Reg_USER_DMA_PUT = 0x40;   // [RW] DMA put address
const uint32_t Reg_USER_DMA_GET = 0x44;   // [RW] DMA get address
const uint32_t Reg_USER_REF = 0x48;       // [RW] DMA reference count

// ----------------------------------------------------------------------------

void USER::Reset() {
}

uint32_t USER::Read(const uint32_t addr) {
    uint32_t channelID = addr >> 16;
    if (m_nv2a.pfifo.GetChannelMode(channelID) == ChannelMode::DMA) {
        if (channelID == m_nv2a.pfifo.GetCurrentChannelID()) {
            switch (addr & 0xffff) {
            case Reg_USER_DMA_PUT:
                return m_nv2a.pfifo.Read(Reg_PFIFO_CACHE1_DMA_PUT);
            case Reg_USER_DMA_GET:
                return m_nv2a.pfifo.Read(Reg_PFIFO_CACHE1_DMA_GET);
            case Reg_USER_REF:
                return m_nv2a.pfifo.Read(Reg_PFIFO_CACHE1_REF);
            default:
                log_spew("[NV2A] USER::Read:   Unimplemented DMA channel read!   address = 0x%x\n", addr);
                return 0;
            }
        }
    }
    log_spew("[NV2A] USER::Read:   Unimplemented read!   address = 0x%x\n", addr);
    return 0;
}

void USER::Write(const uint32_t addr, const uint32_t value) {
    uint32_t channelID = addr >> 16;
    if (m_nv2a.pfifo.GetChannelMode(channelID) == ChannelMode::DMA) {
        if (channelID == m_nv2a.pfifo.GetCurrentChannelID()) {
            switch (addr & 0xffff) {
                case Reg_USER_DMA_PUT:
                    m_nv2a.pfifo.Write(Reg_PFIFO_CACHE1_DMA_PUT, value);
                    break;
                case Reg_USER_DMA_GET:
                    m_nv2a.pfifo.Write(Reg_PFIFO_CACHE1_DMA_GET, value);
                    break;
                case Reg_USER_REF:
                    m_nv2a.pfifo.Write(Reg_PFIFO_CACHE1_REF, value);
                    break;
                default:
                    log_spew("[NV2A] USER::Write:  Unimplemented DMA channel write!   address = 0x%x,  value = 0x%x\n", addr, value);
                    break;
            }
        }
    }
    else {
        log_spew("[NV2A] USER::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
    }
}

}
