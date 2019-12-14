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

    m_delay0 = 0;
    m_dmaTimeslice = 0;

    m_interruptLevels = 0;
    m_enabledInterrupts = 0;

    m_caches = 0;
    m_ramhtParams.u32 = 0;
    m_ramfcParams.u32 = 0;

    m_channelModes = 0;
    m_channelDMA = 0;
    m_channelSizes = 0;

    m_cache0_hash = 0;

    m_cache0_push0Address = 0;

    m_cache1_getAddress = 0;
    m_cache1_putAddress = 0;
    m_cache1_dmaFetch.u32 = 0;
    m_cache1_dmaControl = 0;
    m_cache1_referenceCounter = 0;
    m_cache1_hash = 0;
    m_cache1_acquireTimeout = 0;
    m_cache1_acquireTimestamp = 0;
    m_cache1_acquireValue = 0;
    m_cache1_semaphore = 0;
    m_cache1_status.u32 = 0;
    std::fill(std::begin(m_cache1_commands), std::end(m_cache1_commands), FIFOCommand{ 0 });

    m_dmaPusher.push0.u32 = 0;
    m_dmaPusher.push1.u32 = 0;
    m_dmaPusher.dmaGetAddress = 0;
    m_dmaPusher.dmaPutAddress = 0;
    m_dmaPusher.dmaInstanceAddress = 0;
    m_dmaPusher.dmaPush.u32 = 0;
    m_dmaPusher.dmaState.u32 = 0;
    m_dmaPusher.dmaSubroutine.u32 = 0;
    m_dmaPusher.dcount = 0;
    m_dmaPusher.lastJMPAddress = 0;
    m_dmaPusher.lastCommand = 0;
    m_dmaPusher.lastData = 0;

    m_puller.pull0.u32 = 0;
    m_puller.pull1.u32 = 0;
    m_puller.engines = 0;
}

uint32_t PFIFO::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PFIFO_DELAY_0: return m_delay0;
    case Reg_PFIFO_DMA_TIMESLICE: return m_dmaTimeslice;

    case Reg_PFIFO_INTR: return m_interruptLevels;
    case Reg_PFIFO_INTR_ENABLE: return m_enabledInterrupts;

    case Reg_PFIFO_CACHES: return m_caches;
    case Reg_PFIFO_RAMHT: return m_ramhtParams.u32;
    case Reg_PFIFO_RAMFC: return m_ramfcParams.u32;

    case Reg_PFIFO_MODE: return m_channelModes;
    case Reg_PFIFO_DMA: return m_channelDMA;
    case Reg_PFIFO_SIZE: return m_channelSizes;

    case Reg_PFIFO_CACHE0_PUSH0: return m_cache0_push0Address;
    case Reg_PFIFO_CACHE0_PULL0: return m_cache0_pull0Address;
    case Reg_PFIFO_CACHE0_HASH: return m_cache0_hash;

    case Reg_PFIFO_CACHE1_PUT: return m_cache1_putAddress;
    case Reg_PFIFO_CACHE1_DMA_FETCH: return m_cache1_dmaFetch.u32;
    case Reg_PFIFO_CACHE1_DMA_CTL: return m_cache1_dmaControl;
    case Reg_PFIFO_CACHE1_REF_CNT: return m_cache1_referenceCounter;
    case Reg_PFIFO_CACHE1_HASH: return m_cache1_hash;
    case Reg_PFIFO_CACHE1_ACQUIRE_TIMEOUT: return m_cache1_acquireTimeout;
    case Reg_PFIFO_CACHE1_ACQUIRE_TIMESTAMP: return m_cache1_acquireTimestamp;
    case Reg_PFIFO_CACHE1_ACQUIRE_VALUE: return m_cache1_acquireValue;
    case Reg_PFIFO_CACHE1_SEMAPHORE: return m_cache1_semaphore;
    case Reg_PFIFO_CACHE1_GET: return m_cache1_getAddress;
    case Reg_PFIFO_CACHE1_STATUS: return m_cache1_status.u32;

    case Reg_PFIFO_CACHE1_PUSH0: return m_dmaPusher.push0.u32;
    case Reg_PFIFO_CACHE1_PUSH1: return m_dmaPusher.push1.u32;
    case Reg_PFIFO_CACHE1_DMA_PUSH: return m_dmaPusher.dmaPush.u32;
    case Reg_PFIFO_CACHE1_DMA_STATE: return m_dmaPusher.dmaState.u32;
    case Reg_PFIFO_CACHE1_DMA_PUT: return m_dmaPusher.dmaPutAddress;
    case Reg_PFIFO_CACHE1_DMA_GET: return m_dmaPusher.dmaGetAddress;
    case Reg_PFIFO_CACHE1_DMA_INSTANCE: return m_dmaPusher.dmaInstanceAddress;
    case Reg_PFIFO_CACHE1_DMA_SUBROUTINE: return m_dmaPusher.dmaSubroutine.u32;
    case Reg_PFIFO_CACHE1_DMA_DCOUNT: return m_dmaPusher.dcount;
    case Reg_PFIFO_CACHE1_DMA_GET_JMP_SHADOW: return m_dmaPusher.lastJMPAddress;
    case Reg_PFIFO_CACHE1_DMA_RSVD_SHADOW: return m_dmaPusher.lastCommand;
    case Reg_PFIFO_CACHE1_DMA_DATA_SHADOW: return m_dmaPusher.lastData;

    case Reg_PFIFO_CACHE1_PULL0: return m_puller.pull0.u32;
    case Reg_PFIFO_CACHE1_PULL1: return m_puller.pull1.u32;
    case Reg_PFIFO_CACHE1_ENGINE: return m_puller.engines;

    default:
        if (addr >= Reg_PFIFO_CACHE1_COMMAND_BASE && addr < Reg_PFIFO_CACHE1_COMMAND_BASE + kPFIFO_CommandBufferSize * sizeof(FIFOCommand)) {
            size_t cmdIndex = (addr - Reg_PFIFO_CACHE1_COMMAND_BASE) / sizeof(FIFOCommand);
            size_t offset = ((addr - Reg_PFIFO_CACHE1_COMMAND_BASE) >> 2) & 1;
            return m_cache1_commands[cmdIndex].u32[offset];
        }
        log_spew("[NV2A] PFIFO::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PFIFO::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PFIFO_DELAY_0: m_delay0 = value; break;
    case Reg_PFIFO_DMA_TIMESLICE: m_dmaTimeslice = value; break;

    case Reg_PFIFO_INTR:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PFIFO_INTR_ENABLE:
        m_enabledInterrupts = value;
        m_nv2a.UpdateIRQ();
        break;

    case Reg_PFIFO_CACHES: m_caches = value; break;
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

    case Reg_PFIFO_CACHE0_PUSH0: m_cache0_push0Address = value; break;
    case Reg_PFIFO_CACHE0_PULL0: m_cache0_pull0Address = value; break;
    case Reg_PFIFO_CACHE0_HASH: m_cache0_hash = value; break;

    case Reg_PFIFO_CACHE1_PUT: m_cache1_putAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_FETCH: m_cache1_dmaFetch.u32 = value; break;
    case Reg_PFIFO_CACHE1_DMA_CTL: m_cache1_dmaControl = value; break;
    case Reg_PFIFO_CACHE1_REF_CNT: m_cache1_referenceCounter = value; break;
    case Reg_PFIFO_CACHE1_HASH: m_cache1_hash = value; break;
    case Reg_PFIFO_CACHE1_ACQUIRE_TIMEOUT: m_cache1_acquireTimeout = value; break;
    case Reg_PFIFO_CACHE1_ACQUIRE_TIMESTAMP: m_cache1_acquireTimestamp = value; break;
    case Reg_PFIFO_CACHE1_ACQUIRE_VALUE: m_cache1_acquireValue = value; break;
    case Reg_PFIFO_CACHE1_SEMAPHORE: m_cache1_semaphore = value; break;
    case Reg_PFIFO_CACHE1_GET: m_cache1_getAddress = value; break;
    case Reg_PFIFO_CACHE1_STATUS: m_cache1_status.u32 = value; break;
    
    case Reg_PFIFO_CACHE1_PUSH0: m_dmaPusher.push0.u32 = value; break;
    case Reg_PFIFO_CACHE1_PUSH1: m_dmaPusher.push1.u32 = value; break;
    case Reg_PFIFO_CACHE1_DMA_PUSH: m_dmaPusher.dmaPush.u32 = value; break;
    case Reg_PFIFO_CACHE1_DMA_STATE: m_dmaPusher.dmaState.u32 = value; break;
    case Reg_PFIFO_CACHE1_DMA_PUT: m_dmaPusher.dmaPutAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_GET: m_dmaPusher.dmaGetAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_INSTANCE: m_dmaPusher.dmaInstanceAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_SUBROUTINE: m_dmaPusher.dmaSubroutine.u32 = value; break;
    case Reg_PFIFO_CACHE1_DMA_DCOUNT: m_dmaPusher.dcount = value; break;
    case Reg_PFIFO_CACHE1_DMA_GET_JMP_SHADOW: m_dmaPusher.lastJMPAddress = value; break;
    case Reg_PFIFO_CACHE1_DMA_RSVD_SHADOW: m_dmaPusher.lastCommand = value; break;
    case Reg_PFIFO_CACHE1_DMA_DATA_SHADOW: m_dmaPusher.lastData = value; break;

    case Reg_PFIFO_CACHE1_PULL0: m_puller.pull0.u32 = value; break;
    case Reg_PFIFO_CACHE1_PULL1: m_puller.pull1.u32 = value; break;
    case Reg_PFIFO_CACHE1_ENGINE: m_puller.engines = value; break;

    default:
        if (addr >= Reg_PFIFO_CACHE1_COMMAND_BASE && addr < Reg_PFIFO_CACHE1_COMMAND_BASE + kPFIFO_CommandBufferSize * sizeof(FIFOCommand)) {
            size_t cmdIndex = (addr - Reg_PFIFO_CACHE1_COMMAND_BASE) / sizeof(FIFOCommand);
            size_t offset = ((addr - Reg_PFIFO_CACHE1_COMMAND_BASE) >> 2) & 1;
            m_cache1_commands[cmdIndex].u32[offset] = value;
        }
        else {
            log_spew("[NV2A] PFIFO::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        }
        break;
    }
}

RAMHT::Entry* PFIFO::GetRAMHTEntry(uint32_t handle, uint32_t channelID) {
    uint32_t hash = m_ramhtParams.Hash(handle, channelID);
    uint32_t address = (m_ramhtParams.baseAddress << 12) + (hash << 3);
    return reinterpret_cast<RAMHT::Entry*>(m_nv2a.pramin.GetMemoryPointer(address));
}

}
