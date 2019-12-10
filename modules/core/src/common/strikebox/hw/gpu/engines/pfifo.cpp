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

static inline void printRAMHTParameters(RAMHTParameters& params) {
#if LOG_LEVEL >= LOG_LEVEL_SPEW
    log_spew("[NV2A] PFIFO RAMHT updated:  base addr = 0x%x,  size = ", params.baseAddress);
    switch (params.size) {
    case RAMHTParameters::Size::_4K: log_spew("4K"); break;
    case RAMHTParameters::Size::_8K: log_spew("8K"); break;
    case RAMHTParameters::Size::_16K: log_spew("16K"); break;
    case RAMHTParameters::Size::_32K: log_spew("32K"); break;
    }
    log_spew(",  search = ");
    switch (params.search) {
    case RAMHTParameters::Search::_16: log_spew("16"); break;
    case RAMHTParameters::Search::_32: log_spew("32"); break;
    case RAMHTParameters::Search::_64: log_spew("64"); break;
    case RAMHTParameters::Search::_128: log_spew("128"); break;
    }
    log_spew("\n");
#endif
}

static inline void printRAMFCParameters(RAMFCParameters& params) {
#if LOG_LEVEL >= LOG_LEVEL_SPEW
    log_spew("[NV2A] PFIFO RAMFC updated:  base addr 1 = 0x%x,  base addr 2 = 0x%x,  size = ", params.baseAddress1, params.baseAddress2);
    switch (params.size) {
    case RAMFCParameters::Size::_1K: log_spew("1K\n"); break;
    case RAMFCParameters::Size::_2K: log_spew("2K\n"); break;
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
    m_interruptLevels = 0;
    m_enabledInterrupts = 0;
    m_ramhtParams.u32 = 0;
    m_ramfcParams.u32 = 0;
    // TODO: stop threads
}

uint32_t PFIFO::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PFIFO_INTR: return m_interruptLevels;
    case Reg_PFIFO_INTR_ENABLE: return m_enabledInterrupts;
    case Reg_PFIFO_RAMHT: return m_ramhtParams.u32;
    case Reg_PFIFO_RAMFC: return m_ramfcParams.u32;
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
    default:
        log_spew("[NV2A] PFIFO::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        break;
    }
}

}
