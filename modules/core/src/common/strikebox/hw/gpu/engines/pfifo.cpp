// StrikeBox NV2A PFIFO (MMIO and DMA FIFO submission to PGRAPH) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pfifo.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

static inline void printRAMHTParameters(RAMHT& params) {
#if LOG_LEVEL >= LOG_LEVEL_SPEW
    log_spew("[NV2A] PFIFO RAMHT updated:  base addr = 0x%x,  size = ", params.baseAddress);
    switch (params.size) {
    case RAMHT::Size::_4K: log_spew("4K"); break;
    case RAMHT::Size::_8K: log_spew("8K"); break;
    case RAMHT::Size::_16K: log_spew("16K"); break;
    case RAMHT::Size::_32K: log_spew("32K"); break;
    }
    log_spew(",  search = ");
    switch (params.search) {
    case RAMHT::Search::_16: log_spew("16"); break;
    case RAMHT::Search::_32: log_spew("32"); break;
    case RAMHT::Search::_64: log_spew("64"); break;
    case RAMHT::Search::_128: log_spew("128"); break;
    }
    log_spew("\n");
#endif
}

static inline void printRAMFCParameters(RAMFC& params) {
#if LOG_LEVEL >= LOG_LEVEL_SPEW
    log_spew("[NV2A] PFIFO RAMFC updated:  base addr 1 = 0x%x,  base addr 2 = 0x%x,  size = ", params.baseAddress1, params.baseAddress2);
    switch (params.size) {
    case RAMFC::Size::_1K: log_spew("1K\n"); break;
    case RAMFC::Size::_2K: log_spew("2K\n"); break;
    }
#endif
}

// ----------------------------------------------------------------------------

void PFIFO::SetEnabled(bool enabled) {
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

void PFIFO::Reset() {
    m_enabled = false;
    // TODO: stop threads

    m_interruptLevels = 0;
    m_enabledInterrupts = 0;
    m_ramhtParams.u32 = 0;
    m_ramfcParams.u32 = 0;

    m_channelModes = 0;
    m_channelDMA = 0;
    m_channelSizes = 0;

    m_cache1_getAddress = 0;
    m_cache1_putAddress = 0;
    m_cache1_dmaGetAddress = 0;
    m_cache1_dmaPutAddress = 0;
    m_cache1_dmaState.u32 = 0;

    m_cache1_push0Address = 0;
    m_cache1_pull0Address = 0;

    m_cache1_push1Address = 0;
    m_cache1_pull1Address = 0;
}

uint32_t PFIFO::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PFIFO_INTR: return m_interruptLevels;
    case Reg_PFIFO_INTR_ENABLE: return m_enabledInterrupts;
    case Reg_PFIFO_RAMHT: return m_ramhtParams.u32;
    case Reg_PFIFO_RAMFC: return m_ramfcParams.u32;
    case Reg_PFIFO_MODE: return m_channelModes;
    case Reg_PFIFO_DMA: return m_channelDMA;
    case Reg_PFIFO_SIZE: return m_channelSizes;
    case Reg_PFIFO_CACHE1_PUSH0: return m_cache1_push0Address;
    case Reg_PFIFO_CACHE1_PUSH1: return m_cache1_push1Address;
    case Reg_PFIFO_CACHE1_PUT: return m_cache1_putAddress;
    case Reg_PFIFO_CACHE1_DMA_STATE: return m_cache1_dmaState.u32;
    case Reg_PFIFO_CACHE1_DMA_PUT: return m_cache1_dmaPutAddress;
    case Reg_PFIFO_CACHE1_DMA_GET: return m_cache1_dmaGetAddress;
    case Reg_PFIFO_CACHE1_PULL0: return m_cache1_pull0Address;
    case Reg_PFIFO_CACHE1_PULL1: return m_cache1_pull1Address;
    case Reg_PFIFO_CACHE1_GET: return m_cache1_getAddress;

    default:
        log_spew("[NV2A] PFIFO::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PFIFO::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PFIFO_INTR:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PFIFO_INTR_ENABLE:
        m_enabledInterrupts = value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PFIFO_RAMHT:
        m_ramhtParams.u32 = value;
        printRAMHTParameters(m_ramhtParams);
        break;
    case Reg_PFIFO_RAMFC:
        m_ramfcParams.u32 = value;
        printRAMFCParameters(m_ramfcParams);
        break;
    case Reg_PFIFO_MODE: m_channelModes = value; break;
    case Reg_PFIFO_DMA: m_channelDMA = value; break;
    case Reg_PFIFO_SIZE: m_channelSizes = value; break;
    case Reg_PFIFO_CACHE1_PUSH0: m_cache1_push0Address = value; break;
    case Reg_PFIFO_CACHE1_PUSH1: m_cache1_push1Address = value; break;
    case Reg_PFIFO_CACHE1_PUT: m_cache1_putAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_STATE: m_cache1_dmaState.u32 = value; break;
    case Reg_PFIFO_CACHE1_DMA_PUT: m_cache1_dmaPutAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_GET: m_cache1_dmaGetAddress = value; break;
    case Reg_PFIFO_CACHE1_PULL0: m_cache1_pull0Address = value; break;
    case Reg_PFIFO_CACHE1_PULL1: m_cache1_pull1Address = value; break;
    case Reg_PFIFO_CACHE1_GET: m_cache1_dmaGetAddress = value; break;
    default:
        log_spew("[NV2A] PFIFO::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        break;
    }
}

RAMHT::Entry* PFIFO::GetRAMHTEntry(uint32_t handle, uint32_t channelID) {
    uint32_t hash = m_ramhtParams.Hash(handle, channelID);
    uint32_t address = (m_ramhtParams.baseAddress << 12) + (hash << 3);
    return reinterpret_cast<RAMHT::Entry*>(m_nv2a.pramin.GetMemoryPointer(address));
}

}
