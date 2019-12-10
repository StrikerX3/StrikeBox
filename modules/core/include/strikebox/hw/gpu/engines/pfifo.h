// StrikeBox NV2A PFIFO (MMIO and DMA FIFO submission to PGRAPH) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/fifo/intro.html]
// "Commands to most of the engines are sent through a special engine called PFIFO.
//  PFIFO maintains multiple fully independent command queues, known as "channels" or "FIFO"s.
//  Each channel is controlled through a "channel control area", which is a region of MMIO.
//  PFIFO intercepts all accesses to that area and acts upon them."
//
// PFIFO engine registers occupy the range 0x002000..0x003FFF.
#pragma once

#include "../engine.h"

namespace strikebox::nv2a {

// PFIFO registers
// [https://envytools.readthedocs.io/en/latest/hw/fifo/nv4-pfifo.html#mmio-registers]
// [https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/nouveau/nvkm/engine/fifo/regsnv04.h]
const uint32_t Reg_PFIFO_INTR = 0x100;                         // [RW] Interrupt status
const uint32_t Reg_PFIFO_INTR_ENABLE = 0x140;                  // [RW] Interrupt enable
/**/const uint32_t Val_PFIFO_INTR_CACHE_ERROR = (1 << 0);      //  bit  0: Cache error
/**/const uint32_t Val_PFIFO_INTR_RUNOUT = (1 << 4);           //  bit  4: Runout
/**/const uint32_t Val_PFIFO_INTR_RUNOUT_OVERFLOW = (1 << 8);  //  bit  8: Runout overflow
/**/const uint32_t Val_PFIFO_INTR_DMA_PUSHER = (1 << 12);      //  bit 12: DMA pusher
/**/const uint32_t Val_PFIFO_INTR_DMA_PT = (1 << 16);          //  bit 16: DMA PT
/**/const uint32_t Val_PFIFO_INTR_SEMAPHORE = (1 << 20);       //  bit 20: Semaphore
/**/const uint32_t Val_PFIFO_INTR_ACQUIRE_TIMEOUT = (1 << 24); //  bit 24: Acquire timeout
const uint32_t Reg_PFIFO_RAMHT = 0x210;                        // [RW] RAMHT (hash table) parameters
const uint32_t Reg_PFIFO_RAMFC = 0x214;                        // [RW] RAMFC (FIFO context) parameters

// --- RAMHT parameters ------------

union RAMHTParameters {
    enum class Size : uint32_t { _4K, _8K, _16K, _32K };
    enum class Search : uint32_t { _16, _32, _64, _128 };

    uint32_t u32;
    struct {
        uint32_t _unused1 : 4;     //  3.. 0 = unused
        uint32_t baseAddress : 5;  //  8.. 5 = base address of hash table
        uint32_t _unused2 : 7;     // 15.. 9 = unused
        Size size : 2;             // 17..16 = size of hash table in bytes
        uint32_t _unused3 : 6;     // 23..18 = unused
        Search search : 2;         // 25..24 = entry search stride in bytes
    };
};
static_assert(sizeof(RAMHTParameters) == sizeof(uint32_t));

// --- RAMFC parameters ------------

union RAMFCParameters {
    enum class Size : uint32_t { _1K, _2K };

    uint32_t u32;
    struct {
        uint32_t _unused1 : 2;      //  1.. 0 = unused
        uint32_t baseAddress1 : 7;  //  8.. 2 = base address of FIFO context 1
        uint32_t _unused2 : 7;      // 15.. 9 = unused
        Size size : 1;              // 16..16 = size of FIFO context in bytes
        uint32_t baseAddress2 : 7;  // 23..17 = base address of FIFO context 2
    };
};
static_assert(sizeof(RAMFCParameters) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

// NV2A MMIO and DMA FIFO submission to PGRAPH engine (PFIFO)
class PFIFO : public NV2AEngine {
public:
    PFIFO(NV2A& nv2a) : NV2AEngine("PFIFO", 0x002000, 0x2000, nv2a) {}

    void SetEnabled(bool enabled);

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

    bool GetInterruptState() { return m_interruptLevels & m_enabledInterrupts; }

private:
    bool m_enabled = false;

    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;
    
    RAMHTParameters m_ramhtParams;
    RAMFCParameters m_ramfcParams;
};

}
