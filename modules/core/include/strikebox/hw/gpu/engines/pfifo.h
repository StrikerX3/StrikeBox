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

// envytools documentation on PFIFO is very sparse.
// However, some of the registers seem to be compatible with NV1 which is documented here:
// [https://envytools.readthedocs.io/en/latest/hw/fifo/nv1-pfifo.html#the-mmio-registers]

// Operational parameters
const uint32_t Reg_PFIFO_DELAY_0 = 0x40;                       // [RW] Delay (?)
const uint32_t Reg_PFIFO_DMA_TIMESLICE = 0x44;                 // [RW] DMA timeslice

// Interrupt state
const uint32_t Reg_PFIFO_INTR = 0x100;                         // [RW] Interrupt status
const uint32_t Reg_PFIFO_INTR_ENABLE = 0x140;                  // [RW] Interrupt enable
/**/const uint32_t Val_PFIFO_INTR_CACHE_ERROR = (1 << 0);      //  bit  0: Cache error
/**/const uint32_t Val_PFIFO_INTR_RUNOUT = (1 << 4);           //  bit  4: Runout
/**/const uint32_t Val_PFIFO_INTR_RUNOUT_OVERFLOW = (1 << 8);  //  bit  8: Runout overflow
/**/const uint32_t Val_PFIFO_INTR_DMA_PUSHER = (1 << 12);      //  bit 12: DMA pusher
/**/const uint32_t Val_PFIFO_INTR_DMA_PT = (1 << 16);          //  bit 16: DMA PT
/**/const uint32_t Val_PFIFO_INTR_SEMAPHORE = (1 << 20);       //  bit 20: Semaphore
/**/const uint32_t Val_PFIFO_INTR_ACQUIRE_TIMEOUT = (1 << 24); //  bit 24: Acquire timeout

// Memory and cache parameters
const uint32_t Reg_PFIFO_RAMHT = 0x210;                        // [RW] RAMHT (hash table) parameters
const uint32_t Reg_PFIFO_RAMFC = 0x214;                        // [RW] RAMFC (FIFO context) parameters
const uint32_t Reg_PFIFO_CACHES = 0x500;                       // [RW] PFIFO caches (?)

// Channel flags
// Each bit in these registers represents a channel (bit 0 = channel 0, bit 31 = channel 31)
const uint32_t Reg_PFIFO_MODE = 0x504;                         // [RW] PFIFO channel modes (0 = PIO, 1 = DMA)
const uint32_t Reg_PFIFO_DMA = 0x508;                          // [RW] PFIFO DMA (?)
const uint32_t Reg_PFIFO_SIZE = 0x50c;                         // [RW] PFIFO size (?)

// Cache 0 registers
// Most of these are assumptions based on the expectation that CACHE0 registers at 0x1000..0x11FF
// match the corresponding CACHE1 registers at 0x1200..0x13FF
const uint32_t Reg_PFIFO_CACHE0_PUSH0 = 0x1000;                // [RW] Cache 0 pusher parameters #0 (PFIFOCachePush0Parameters)
const uint32_t Reg_PFIFO_CACHE0_PULL0 = 0x1050;                // [RW] Cache 0 puller parameters #0 (PFIFOCachePull0Parameters)
const uint32_t Reg_PFIFO_CACHE0_HASH = 0x1058;                 // [RW] Cache 0 hash (?)

// Cache 1 registers
const uint32_t Reg_PFIFO_CACHE1_PUSH0 = 0x1200;                // [RW] Cache 1 pusher parameters #0 (PFIFOCachePush0Parameters)
const uint32_t Reg_PFIFO_CACHE1_PUSH1 = 0x1204;                // [RW] Cache 1 pusher parameters #1 (PFIFOCachePush1Parameters)
const uint32_t Reg_PFIFO_CACHE1_PUT = 0x1210;                  // [RW] Cache 1 put address
const uint32_t Reg_PFIFO_CACHE1_STATUS = 0x1214;               // [RW] Cache 1 (puller/pusher) status (PFIFOCacheStatus)
const uint32_t Reg_PFIFO_CACHE1_DMA_PUSH = 0x1220;             // [RW] Cache 1 (pusher) DMA push state (PFIFOCacheDMAPush)
const uint32_t Reg_PFIFO_CACHE1_DMA_FETCH = 0x1224;            // [RW] Cache 1 DMA fetch parameters (PFIFOCacheDMAFetch)
const uint32_t Reg_PFIFO_CACHE1_DMA_STATE = 0x1228;            // [RW] Cache 1 (pusher) DMA state (PFIFOPusherDMAState)
const uint32_t Reg_PFIFO_CACHE1_DMA_INSTANCE = 0x122c;         // [RW] Cache 1 (pusher) DMA instance address
const uint32_t Reg_PFIFO_CACHE1_DMA_CTL = 0x1230;              // [RW] Cache 1 DMA control
const uint32_t Reg_PFIFO_CACHE1_DMA_PUT = 0x1240;              // [RW] Cache 1 (pusher) DMA put address
const uint32_t Reg_PFIFO_CACHE1_DMA_GET = 0x1244;              // [RW] Cache 1 (pusher) DMA get address
const uint32_t Reg_PFIFO_CACHE1_REF_CNT = 0x1248;              // [RW] Cache 1 (pusher/puller) reference counter
const uint32_t Reg_PFIFO_CACHE1_DMA_SUBROUTINE = 0x124c;       // [RW] Cache 1 (pusher) DMA subroutine status (PFIFODMASubroutine)
const uint32_t Reg_PFIFO_CACHE1_PULL0 = 0x1250;                // [RW] Cache 1 puller parameters #0 (PFIFOCachePull0Parameters)
const uint32_t Reg_PFIFO_CACHE1_PULL1 = 0x1254;                // [RW] Cache 1 puller parameters #1 (PFIFOCachePull1Parameters)
const uint32_t Reg_PFIFO_CACHE1_HASH = 0x1258;                 // [RW] Cache 1 hash (?)
const uint32_t Reg_PFIFO_CACHE1_ACQUIRE_TIMEOUT = 0x1260;      // [RW] Cache 1 (puller) semaphore acquire timeout
const uint32_t Reg_PFIFO_CACHE1_ACQUIRE_TIMESTAMP = 0x1264;    // [RW] Cache 1 (puller) semaphore acquire timestamp
const uint32_t Reg_PFIFO_CACHE1_ACQUIRE_VALUE = 0x1268;        // [RW] Cache 1 (puller) semaphore acquire value
const uint32_t Reg_PFIFO_CACHE1_SEMAPHORE = 0x126c;            // [RW] Cache 1 (puller) semaphore
const uint32_t Reg_PFIFO_CACHE1_GET = 0x1270;                  // [RW] Cache 1 get address
const uint32_t Reg_PFIFO_CACHE1_ENGINE = 0x1280;               // [RW] Cache 1 (puller) engine modes (FIFOEngine) (one channel every 4 bits)
const uint32_t Reg_PFIFO_CACHE1_DMA_DCOUNT = 0x12a0;           // [RW] Cache 1 (pusher) DMA error - number of dwords transferred
const uint32_t Reg_PFIFO_CACHE1_DMA_GET_JMP_SHADOW = 0x12a4;   // [RW] Cache 1 (pusher) DMA error - last JMP address
const uint32_t Reg_PFIFO_CACHE1_DMA_RSVD_SHADOW = 0x12a8;      // [RW] Cache 1 (pusher) DMA error - last command
const uint32_t Reg_PFIFO_CACHE1_DMA_DATA_SHADOW = 0x12ac;      // [RW] Cache 1 (pusher) DMA error - last data

const uint32_t Reg_PFIFO_CACHE1_COMMAND_BASE = 0x1800;         //      Base address of FIFO commands (FIFOCommand)
const uint32_t Reg_PFIFO_CACHE1_METHOD = 0x0;                  // [RW] Offset of FIFO command methods
const uint32_t Reg_PFIFO_CACHE1_DATA = 0x4;                    // [RW] Offset of FIFO command parameters
const size_t kPFIFO_CommandBufferSize = 128;

// ----------------------------------------------------------------------------

union PFIFOCachePush0Parameters {
    enum class Access : uint32_t { Disabled, Enabled };

    uint32_t u32;
    struct {
        Access access : 1;   //  0.. 0 = access
    };
};
static_assert(sizeof(PFIFOCachePush0Parameters) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOCachePush1Parameters {
    enum class Mode : uint32_t { PIO, DMA };

    uint32_t u32;
    struct {
        uint32_t
            channelID : 4,   //  3.. 0 = channel ID
            : 4;             //  7.. 4 = unused
        Mode mode : 1;       //  8.. 8 = mode
    };
};
static_assert(sizeof(PFIFOCachePush1Parameters) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOCachePull0Parameters {
    enum class Access : uint32_t { Disabled, Enabled };

    uint32_t u32;
    struct {
        Access access : 1;   //  0.. 0 = access
    };
};
static_assert(sizeof(PFIFOCachePull0Parameters) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOCachePull1Parameters {
    uint32_t u32;
    struct {
        FIFOEngine engine : 2;   //  1.. 0 = engine
    };
};
static_assert(sizeof(PFIFOCachePull1Parameters) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOCacheDMAPush {
    enum class Access : uint32_t { Disabled, Enabled };
    enum class State : uint32_t { Idle, Busy };
    enum class Buffer : uint32_t { NotEmpty, Empty };
    enum class Status : uint32_t { Running, Suspended };
    enum class Acquire : uint32_t { NotPending, Pending };

    uint32_t u32;
    struct {
        Access access : 1,     //  0.. 0 = access
            : 3;               //  3.. 1 = unused
        State state : 1,       //  4.. 4 = push state
            : 3;               //  7.. 5 = unused
        Buffer buffer : 1,     //  8.. 8 = buffer state
            : 3;               // 11.. 9 = unused
        Status status : 1,     // 12..12 = status
            : 3;               // 15..13 = unused
        Acquire acquire : 1;   // 16..16 = acquire state
    };
};
static_assert(sizeof(PFIFOCacheDMAPush) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOCacheStatus {
    uint32_t u32;
    struct {
        uint32_t
            : 4,           //  3.. 0 = unused
            lowMark : 1,   //  4.. 4 = low mark  (1 = empty)
            : 3,           //  7.. 5 = unused
            highMark : 1;  //  8.. 8 = high mark (1 = full)
    };
};
static_assert(sizeof(PFIFOCacheStatus) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOCacheDMAFetch {
    uint32_t u32;
    struct {
        uint32_t
            : 3,                 //
            fetchTrigger : 5,    // Fetch trigger (multiples of 8; 0 = 8, 31 = 256)
            : 5,                 //
            fetchSize : 3,       // Fetch size (multiples of 32; 0 = 32, 7 = 256)
            maxReqs : 4;         // Maximum number of requests (0 to 15)
    };
};
static_assert(sizeof(PFIFOCacheDMAFetch) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFOPusherDMAState {
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
static_assert(sizeof(PFIFOPusherDMAState) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union PFIFODMASubroutine {
    enum class State : uint32_t { Inactive, Active };

    uint32_t u32;
    struct {
        State enabled : 1,           //  0.. 0 = state
            : 1;                     //  1.. 1 = unused
        uint32_t returnOffset : 30;  // 31.. 2 = return offset (top 30 bits; all LSBs are zero)
    };
};
static_assert(sizeof(PFIFODMASubroutine) == sizeof(uint32_t));

// ----------------------------------------------------------------------------

union FIFOCommand {
    enum class MethodType : uint32_t { Incrementing, NonIncrementing };

    uint32_t u32[2];
    struct {
        // METHOD
        MethodType type : 1,       //  0.. 0 = method type
            : 1;                   //  1.. 1 = unused
        uint32_t address : 11,     // 12.. 2 = address
            subchannel : 3,        // 15..13 = subchannel
            : 16;                  // 31..16 = unused

        // DATA
        uint32_t data;             // 31.. 0 = data
    };
};
static_assert(sizeof(FIFOCommand) == sizeof(uint32_t[2]));

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

    // Operational parameters
    uint32_t m_delay0;
    uint32_t m_dmaTimeslice;

    // Interrupt state
    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;
    
    // Memory and cache parameters
    RAMHT m_ramhtParams;
    RAMFC m_ramfcParams;
    uint32_t m_caches;

    // Channel flags
    uint32_t m_channelModes;
    uint32_t m_channelDMA;
    uint32_t m_channelSizes;

    // Cache 0 registers
    uint32_t m_cache0_hash;

    uint32_t m_cache0_push0Address;
    uint32_t m_cache0_pull0Address;

    // Cache 1 registers
    // FIXME: some of these are actually part of the PFIFO pusher or puller states and shouldn't be here
    // [https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#pusher-state]
    // [https://envytools.readthedocs.io/en/latest/hw/fifo/puller.html#puller-state]
    uint32_t m_cache1_getAddress;
    uint32_t m_cache1_putAddress;
    PFIFOCacheDMAFetch m_cache1_dmaFetch;
    uint32_t m_cache1_dmaControl;
    uint32_t m_cache1_referenceCounter;
    uint32_t m_cache1_hash;
    uint32_t m_cache1_acquireTimeout;
    uint32_t m_cache1_acquireTimestamp;
    uint32_t m_cache1_acquireValue;
    uint32_t m_cache1_semaphore;
    PFIFOCacheStatus m_cache1_status;
    FIFOCommand m_cache1_commands[kPFIFO_CommandBufferSize];

    // DMA pusher state
    struct DMAPusher {
        PFIFOCachePush0Parameters push0;
        PFIFOCachePush1Parameters push1;

        uint32_t dmaGetAddress;
        uint32_t dmaPutAddress;
        uint32_t dmaInstanceAddress;

        PFIFODMASubroutine dmaSubroutine;
        PFIFOCacheDMAPush dmaPush;
        PFIFOPusherDMAState dmaState;

        uint32_t dcount;
        uint32_t lastJMPAddress;
        uint32_t lastCommand;
        uint32_t lastData;
    } m_dmaPusher;

    // Puller state
    struct Puller {
        PFIFOCachePull0Parameters pull0;
        PFIFOCachePull1Parameters pull1;

        uint32_t engines;
    } m_puller;
};

}
