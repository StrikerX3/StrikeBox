// StrikeBox NV2A PGRAPH (2D/3D graphics engine) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pgraph.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PGRAPH::SetEnabled(bool enabled) {
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

void PGRAPH::Reset() {
    m_enabled = false;
    // TODO: stop threads
 
    m_interruptLevels = 0;
    m_enabledInterrupts = 0;

    m_ctxControl.u32 = 0;
    std::fill(std::begin(m_ctxSwitches), std::end(m_ctxSwitches), 0);
    m_fifoEnabled = false;
    m_status = 0;

    m_rdiIndex = 0;

    m_ffintfc_st2 = 0;

    m_channelCtxTable = 0;
    m_channelCtxPointer = 0;

    std::fill(std::begin(m_tiles), std::end(m_tiles), PGRAPHTile{ 0 });
    std::fill(std::begin(m_zcomp), std::end(m_zcomp), 0);
}

uint32_t PGRAPH::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PGRAPH_INTR: return m_interruptLevels;
    case Reg_PGRAPH_INTR_ENABLE: return m_enabledInterrupts;

    case Reg_PGRAPH_CTX_CONTROL: return m_ctxControl.u32;
    case Reg_PGRAPH_CTX_SWITCH1: return m_ctxSwitches[0];
    case Reg_PGRAPH_CTX_SWITCH2: return m_ctxSwitches[1];
    case Reg_PGRAPH_CTX_SWITCH3: return m_ctxSwitches[2];
    case Reg_PGRAPH_CTX_SWITCH4: return m_ctxSwitches[3];
    case Reg_PGRAPH_CTX_SWITCH5: return m_ctxSwitches[4];
    case Reg_PGRAPH_STATUS: return m_status;
    case Reg_PGRAPH_FIFO: return m_fifoEnabled ? 1 : 0;
    case Reg_PGRAPH_RDI_INDEX: return m_rdiIndex;
    case Reg_PGRAPH_RDI_DATA: return RDIRead();
    case Reg_PGRAPH_FFINTFC_ST2: return m_ffintfc_st2;
    case Reg_PGRAPH_CHANNEL_CTX_TABLE: return m_channelCtxTable;
    case Reg_PGRAPH_CHANNEL_CTX_POINTER: return m_channelCtxPointer;

    default:
        if (addr >= Reg_PGRAPH_TILE_BASE && addr < Reg_PGRAPH_TILE_BASE + kPGRAPH_NumTiles * sizeof(PGRAPHTile)) {
            uint32_t tileIndex = ((addr - Reg_PGRAPH_TILE_BASE) >> 4) & 7;
            switch (addr & 0xF) {
            case Reg_PGRAPH_TILE: return m_tiles[tileIndex].address;
            case Reg_PGRAPH_TLIMIT: return m_tiles[tileIndex].limit;
            case Reg_PGRAPH_TSIZE: return m_tiles[tileIndex].pitch;
            case Reg_PGRAPH_TSTATUS: return m_tiles[tileIndex].status;
            }
        }
        else if (addr >= Reg_PGRAPH_ZCOMP_BASE && addr < Reg_PGRAPH_ZCOMP_BASE + kPGRAPH_NumZCOMP * sizeof(uint32_t)) {
            uint32_t zcompIndex = ((addr - Reg_PGRAPH_ZCOMP_BASE) >> 2) & (kPGRAPH_NumZCOMP - 1);
            return m_zcomp[zcompIndex];
        }
        log_spew("[NV2A] PGRAPH::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PGRAPH::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PGRAPH_STATUS:
        // Discard writes to read-only registers
        break;

    case Reg_PGRAPH_INTR:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PGRAPH_INTR_ENABLE:
        m_enabledInterrupts = value;
        m_nv2a.UpdateIRQ();
        break;

    case Reg_PGRAPH_CTX_CONTROL: m_ctxControl.u32 = value; break;
    case Reg_PGRAPH_CTX_SWITCH1: m_ctxSwitches[0] = value; break;
    case Reg_PGRAPH_CTX_SWITCH2: m_ctxSwitches[1] = value; break;
    case Reg_PGRAPH_CTX_SWITCH3: m_ctxSwitches[2] = value; break;
    case Reg_PGRAPH_CTX_SWITCH4: m_ctxSwitches[3] = value; break;
    case Reg_PGRAPH_CTX_SWITCH5: m_ctxSwitches[4] = value; break;
    case Reg_PGRAPH_FIFO: m_fifoEnabled = value & 1; break;
    case Reg_PGRAPH_RDI_INDEX: m_rdiIndex = value; break;
    case Reg_PGRAPH_RDI_DATA: RDIWrite(value); break;
    case Reg_PGRAPH_FFINTFC_ST2: m_ffintfc_st2 = value; break;
    case Reg_PGRAPH_CHANNEL_CTX_TABLE: m_channelCtxTable = value; break;
    case Reg_PGRAPH_CHANNEL_CTX_POINTER: m_channelCtxPointer = value; break;

    default:
        if (addr >= Reg_PGRAPH_TILE_BASE && addr < Reg_PGRAPH_TILE_BASE + kPGRAPH_NumTiles * sizeof(PGRAPHTile)) {
            uint32_t tileIndex = (addr - Reg_PGRAPH_TILE_BASE) / sizeof(PGRAPHTile);
            switch (addr & 0xF) {
            case Reg_PGRAPH_TILE: m_tiles[tileIndex].address = value; break;
            case Reg_PGRAPH_TLIMIT: m_tiles[tileIndex].limit = value; break;
            case Reg_PGRAPH_TSIZE: m_tiles[tileIndex].pitch = value; break;
            case Reg_PGRAPH_TSTATUS: m_tiles[tileIndex].status = value; break;
            }
        }
        else if (addr >= Reg_PGRAPH_ZCOMP_BASE && addr < Reg_PGRAPH_ZCOMP_BASE + kPGRAPH_NumZCOMP * sizeof(uint32_t)) {
            uint32_t zcompIndex = (addr - Reg_PGRAPH_ZCOMP_BASE) / sizeof(uint32_t);
            m_zcomp[zcompIndex] = value;
        }
        else {
            log_spew("[NV2A] PGRAPH::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        }
        break;
    }
}

uint32_t PGRAPH::RDIRead() {
    // TODO: implement known RDI registers
    switch (m_rdiIndex) {
    default:
        log_spew("[NV2A] PGRAPH::RDIRead:   Unimplemented RDI read!   index = 0x%x\n", m_rdiIndex);
        return 0;
    }
}

uint32_t PGRAPH::RDIWrite(const uint32_t value) {
    // TODO: implement known RDI registers
    switch (m_rdiIndex) {
    default:
        log_spew("[NV2A] PGRAPH::RDIWrite:  Unimplemented RDI write!   index = 0x%x,  value = 0x%x\n", m_rdiIndex, value);
        return 0;
    }
}

}
