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
const uint32_t Reg_PFIFO_RAMHT = 0x210;                        // [RW] RAMHT (hash table) address
const uint32_t Reg_PFIFO_RAMFC = 0x214;                        // [RW] RAMFC (FIFO context) address


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
    
    uint32_t m_ramhtParams;
    uint32_t m_ramfcParams;
};

}
