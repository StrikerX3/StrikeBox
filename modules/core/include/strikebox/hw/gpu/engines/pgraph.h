// StrikeBox NV2A PGRAPH (2D/3D graphics engine) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// PGRAPH is the main engine responsible for 2D/3D graphics command processing.
//
// PGRAPH engine registers occupy the range 0x400000..0x401FFF.
#pragma once

#include "../engine.h"

namespace strikebox::nv2a {

// PGRAPH registers
// [https://envytools.readthedocs.io/en/latest/hw/graph/kelvin/pgraph.html#mmio-registers]
// [https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/nouveau/nvkm/engine/gr/regs.h]
const uint32_t Reg_PGRAPH_INTR = 0x100;                 // [RW] Interrupt status
const uint32_t Reg_PGRAPH_INTR_ENABLE = 0x140;          // [RW] Interrupt enable
/**/const uint32_t Reg_PGRAPH_INTR_NOTIFY = (1 << 0);               // bit  0: notify
/**/const uint32_t Reg_PGRAPH_INTR_MISSING_HW = (1 << 4);           // bit  4: missing hardware
/**/const uint32_t Reg_PGRAPH_INTR_TLB_PRESENT_DMA_R = (1 << 6);    // bit  6: DMA read TLB not present or invalid
/**/const uint32_t Reg_PGRAPH_INTR_TLB_PRESENT_DMA_W = (1 << 7);    // bit  7: DMA write TLB not present or invalid
/**/const uint32_t Reg_PGRAPH_INTR_TLB_PRESENT_TEX_A = (1 << 8);    // bit  8: texture A TLB not present or invalid
/**/const uint32_t Reg_PGRAPH_INTR_TLB_PRESENT_TEX_B = (1 << 9);    // bit  9: texture B TLB not present or invalid
/**/const uint32_t Reg_PGRAPH_INTR_TLB_PRESENT_VTX = (1 << 10);     // bit 10: vertex TLB not present or invalid
/**/const uint32_t Reg_PGRAPH_INTR_CONTEXT_SWITCH = (1 << 12);      // bit 12: context switch
/**/const uint32_t Reg_PGRAPH_INTR_STATE3D = (1 << 13);             // bit 13: 3D state
/**/const uint32_t Reg_PGRAPH_INTR_BUFFER_NOTIFY = (1 << 16);       // bit 16: buffer notify
/**/const uint32_t Reg_PGRAPH_INTR_ERROR = (1 << 20);               // bit 20: error
/**/const uint32_t Reg_PGRAPH_INTR_SINGLE_STEP = (1 << 24);         // bit 24: single step complete

const uint32_t Reg_PGRAPH_CTX_CONTROL = 0x144;          // [RW] Context control (PGRAPHContextControl)
const uint32_t Reg_PGRAPH_CTX_SWITCH1 = 0x14c;          // [RW] Context switch 1
const uint32_t Reg_PGRAPH_CTX_SWITCH2 = 0x150;          // [RW] Context switch 2
const uint32_t Reg_PGRAPH_CTX_SWITCH3 = 0x154;          // [RW] Context switch 3
const uint32_t Reg_PGRAPH_CTX_SWITCH4 = 0x158;          // [RW] Context switch 4
const uint32_t Reg_PGRAPH_CTX_SWITCH5 = 0x15c;          // [RW] Context switch 5
const uint32_t Reg_PGRAPH_STATUS = 0x700;               // [R ] PGRAPH status
const uint32_t Reg_PGRAPH_FIFO = 0x720;                 // [RW] FIFO state (bit 0: 0 = disabled, 1 = enabled)
const uint32_t Reg_PGRAPH_RDI_INDEX = 0x750;            // [RW] RDI index (RDIIndex)
const uint32_t Reg_PGRAPH_RDI_DATA = 0x754;             // [RW] RDI data
const uint32_t Reg_PGRAPH_FFINTFC_ST2 = 0x764;          // [RW] ??
const uint32_t Reg_PGRAPH_CHANNEL_CTX_TABLE = 0x780;    // [RW] Channel context table (low 16 bits only)
const uint32_t Reg_PGRAPH_CHANNEL_CTX_POINTER = 0x784;  // [RW] Channel context pointer (low 16 bits only)

const uint32_t Reg_PGRAPH_TILE_BASE = 0x900;            //      Base address of tiles
const uint32_t Reg_PGRAPH_TILE = 0x0;                   // [RW]   Tile address
const uint32_t Reg_PGRAPH_TLIMIT = 0x4;                 // [RW]   Tile limit
const uint32_t Reg_PGRAPH_TSIZE = 0x8;                  // [RW]   Tile size (pitch)
const uint32_t Reg_PGRAPH_TSTATUS = 0xc;                // [RW]   Tile status
const size_t kPGRAPH_NumTiles = 8;

const uint32_t Reg_PGRAPH_ZCOMP_BASE = 0x980;           //      Base address of ZCOMP (?)
const size_t kPGRAPH_NumZCOMP = 8;

// ----------------------------------------------------------------------------

// --- RDI -------------------------

union RDIIndex {
    uint32_t u32;
    struct {
        uint32_t
            : 2,           //  1.. 0 = unused
            address : 11,  // 12.. 2 = address
            : 3,           // 15..13 = unused
            select : 9;    // 24..16 = select
    };
};
static_assert(sizeof(RDIIndex) == sizeof(uint32_t));

// --- PGRAPH context control ------

union PGRAPHContextControl {
    uint32_t u32;
    struct {
        uint32_t
            minimumTime : 2,    //  1.. 0 = minimum time (?)
            : 6,                //  7.. 2 = unused
            time : 1,           //  8.. 8 = time (?)
            : 7,                // 15.. 9 = unused
            channelValid : 1,   // 16..16 = channel is valid? (1 = yes)
            : 3,                // 19..17 = unused
            change : 1,         // 20..20 = change (?)
            : 3,                // 23..21 = unused
            switching : 1,      // 24..24 = switching (?)
            : 3,                // 27..25 = unused
            device : 1;         // 28..28 = device (?)
    };
};
static_assert(sizeof(PGRAPHContextControl) == sizeof(uint32_t));

// --- PGRAPH tile -----------------

union PGRAPHTile {
    uint32_t u32[4];
    struct {
        uint32_t address;
        uint32_t limit;
        uint32_t pitch;
        uint32_t status;
    };
};
static_assert(sizeof(PGRAPHTile) == sizeof(uint32_t[4]));

// ----------------------------------------------------------------------------

// NV2A 2D/3D graphics engine (PGRAPH)
class PGRAPH : public NV2AEngine {
public:
    PGRAPH(NV2A& nv2a) : NV2AEngine("PGRAPH", 0x400000, 0x2000, nv2a) {}

    void SetEnabled(bool enabled);
    
    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

    bool GetInterruptState() { return m_interruptLevels & m_enabledInterrupts; }

private:
    bool m_enabled = false;

    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;

    PGRAPHContextControl m_ctxControl;
    uint32_t m_ctxSwitches[5];
    bool m_fifoEnabled;
    uint32_t m_status;

    uint32_t m_rdiIndex;

    uint32_t m_ffintfc_st2;

    uint16_t m_channelCtxTable;
    uint16_t m_channelCtxPointer;

    PGRAPHTile m_tiles[kPGRAPH_NumTiles];
    uint32_t m_zcomp[kPGRAPH_NumZCOMP];

    uint32_t RDIRead();
    uint32_t RDIWrite(const uint32_t value);
};

}
