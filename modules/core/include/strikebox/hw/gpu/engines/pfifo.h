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

// Channel flags
// Each bit in these registers represents a channel (bit 0 = channel 0, bit 31 = channel 31)
const uint32_t Reg_PFIFO_MODE = 0x504;                         // [RW] PFIFO channel modes (0 = PIO, 1 = DMA)
const uint32_t Reg_PFIFO_DMA = 0x508;                          // [RW] PFIFO DMA ??
const uint32_t Reg_PFIFO_SIZE = 0x50c;                         // [RW] PFIFO size ??

// Cache 1 registers
const uint32_t Reg_PFIFO_CACHE1_PUSH0 = 0x1200;                // [RW] Cache 1 pusher 0 address
const uint32_t Reg_PFIFO_CACHE1_PUSH1 = 0x1204;                // [RW] Cache 1 pusher 1 address
const uint32_t Reg_PFIFO_CACHE1_PUT = 0x1210;                  // [RW] Cache 1 put address
const uint32_t Reg_PFIFO_CACHE1_DMA_STATE = 0x1228;            // [RW] Cache 1 DMA state
const uint32_t Reg_PFIFO_CACHE1_DMA_PUT = 0x1240;              // [RW] Cache 1 DMA put address
const uint32_t Reg_PFIFO_CACHE1_DMA_GET = 0x1244;              // [RW] Cache 1 DMA get address
const uint32_t Reg_PFIFO_CACHE1_PULL0 = 0x1250;                // [RW] Cache 1 puller 0 address
const uint32_t Reg_PFIFO_CACHE1_PULL1 = 0x1254;                // [RW] Cache 1 puller 1 address
const uint32_t Reg_PFIFO_CACHE1_GET = 0x1270;                  // [RW] Cache 1 get address

union PFIFOCacheDMAState {
    enum class ErrorCode : uint32_t { None, Call, NonCache, Return, ReservedCommand, Protection };

    uint32_t u32;
    struct {
        uint32_t
            methodType : 1,      // Method type (0 = increasing, 1 = non-increasing)
            : 1,                 // 
            method : 11,         // Method
            subchannel : 3,      // Subchannel
            : 2,                 // 
            methodCount : 11;    // Method count
        ErrorCode error : 3;     // Error code
    };
};
static_assert(sizeof(PFIFOCacheDMAState) == sizeof(uint32_t));

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

    RAMHT::Entry* GetRAMHTEntry(uint32_t handle, uint32_t channelID);

private:
    bool m_enabled = false;

    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;
    
    RAMHT m_ramhtParams;
    RAMFC m_ramfcParams;

    // Channel flags
    uint32_t m_channelModes;
    uint32_t m_channelDMA;
    uint32_t m_channelSizes;

    // Cache 1 registers
    uint32_t m_cache1_getAddress;
    uint32_t m_cache1_putAddress;
    uint32_t m_cache1_dmaGetAddress;
    uint32_t m_cache1_dmaPutAddress;
    PFIFOCacheDMAState m_cache1_dmaState;
    
    uint32_t m_cache1_push0Address;
    uint32_t m_cache1_pull0Address;
    
    uint32_t m_cache1_push1Address;
    uint32_t m_cache1_pull1Address;
};

}
