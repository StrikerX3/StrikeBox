#include "nv2a.h"
#include "openxbox/log.h"
#include "openxbox/thread.h"

#include <cassert>
#include <cstring>

#ifdef _MSC_VER
#include <intrin.h>
static inline int ffs(int i) {
    unsigned long pos;
    if (_BitScanForward(&pos, i)) {
        return pos + 1;
    }
    return 0;
}
#endif

namespace openxbox {

#define GET_MASK(v, mask) (((v) & (mask)) >> (ffs(mask)-1))

#define SET_MASK(v, mask, val) \
    do { \
        (v) &= ~(mask); \
        (v) |= ((val) << (ffs(mask)-1)) & (mask); \
    } while (0)

static inline uint32_t ldl_le_p(const void *p) {
    return *(uint32_t*)p;
}
#define CASE_4(v, step) \
    case (v): \
	case (v)+(step): \
	case (v)+(step) * 2: \
	case (v)+(step) * 3


NV2ADevice::NV2ADevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID,
    uint8_t *pSystemRAM, uint32_t systemRAMSize,
    i8259 *pic)
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
		0x03, 0x00, 0x00) // VGA-compatible controller
    , m_pSystemRAM(pSystemRAM)
    , m_systemRAMSize(systemRAMSize)
    , m_pic(pic)
{
}

NV2ADevice::~NV2ADevice() {
    m_running = false;

    m_PFIFO.cache1.cache_cond.notify_all();

    m_PFIFO.puller_thread.join();
    m_VblankThread.join();
}

// PCI Device functions

void NV2ADevice::Init() {
	RegisterBAR(0, 16 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0xFD000000 - 0xFDFFFFFF
	RegisterBAR(1, 128 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0xF0000000 - 0xF7FFFFFF
	// TODO: check if this is correct
	RegisterBAR(2, 64 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0xF8000000 - 0xFBFFFFFF

    Write8(m_configSpace, PCI_INTERRUPT_PIN, 3);

    m_PCRTC.pendingInterrupts = 0;
    m_PCRTC.enabledInterrupts = 0;

    m_PRAMDAC.core_clock_coeff = 0x00011c01; /* 189MHz...? */
    m_PRAMDAC.core_clock_freq = 189000000;
    m_PRAMDAC.memory_clock_coeff = 0;
    m_PRAMDAC.video_clock_coeff = 0x0003C20D; /* 25182Khz...? */

    Reset();
 
    m_running = true;
    m_PFIFO.puller_thread = std::thread(PFIFO_Puller_Thread, this);
    m_VblankThread = std::thread(VBlankThread, this);
}

void NV2ADevice::Reset() {
    // RAMIN is just RAM, so we allocate it as such
    if (m_pRAMIN == nullptr) {
        m_pRAMIN = (uint8_t*)malloc(NV_PRAMIN_SIZE);
    }

    // VRAM IS System RAM, so we mark it as such
    m_VRAM = m_pSystemRAM;

    memset(m_pRAMIN, 0, NV_PRAMIN_SIZE);

    m_MemoryRegions.clear();
    m_MemoryRegions.push_back({ NV_PMC_ADDR, NV_PMC_SIZE, PMCRead, PMCWrite });
    m_MemoryRegions.push_back({ NV_PBUS_ADDR, NV_PBUS_SIZE, PBUSRead, PBUSWrite });
    m_MemoryRegions.push_back({ NV_PFIFO_ADDR, NV_PFIFO_SIZE, PFIFORead, PFIFOWrite });
    m_MemoryRegions.push_back({ NV_PRMA_ADDR, NV_PRMA_SIZE, PRMARead, PRMAWrite });
    m_MemoryRegions.push_back({ NV_PVIDEO_ADDR, NV_PVIDEO_SIZE, PVIDEORead, PVIDEOWrite });
    m_MemoryRegions.push_back({ NV_PTIMER_ADDR, NV_PTIMER_SIZE, PTIMERRead, PTIMERWrite });
    m_MemoryRegions.push_back({ NV_PCOUNTER_ADDR, NV_PCOUNTER_SIZE, PCOUNTERRead, PCOUNTERWrite });
    m_MemoryRegions.push_back({ NV_PVPE_ADDR, NV_PVPE_SIZE, PVPERead, PVPEWrite });
    m_MemoryRegions.push_back({ NV_PTV_ADDR, NV_PTV_SIZE, PTVRead, PTVWrite });
    m_MemoryRegions.push_back({ NV_PRMFB_ADDR, NV_PRMFB_SIZE, PRMFBRead, PRMFBWrite });
    m_MemoryRegions.push_back({ NV_PRMVIO_ADDR, NV_PRMVIO_SIZE, PRMVIORead, PRMVIOWrite });
    m_MemoryRegions.push_back({ NV_PFB_ADDR, NV_PFB_SIZE, PFBRead, PFBWrite });
    m_MemoryRegions.push_back({ NV_PSTRAPS_ADDR, NV_PSTRAPS_SIZE, PSTRAPSRead, PSTRAPSWrite });
    m_MemoryRegions.push_back({ NV_PGRAPH_ADDR, NV_PGRAPH_SIZE, PGRAPHRead, PGRAPHWrite });
    m_MemoryRegions.push_back({ NV_PCRTC_ADDR, NV_PCRTC_SIZE, PCRTCRead, PCRTCWrite });
    m_MemoryRegions.push_back({ NV_PRMCIO_ADDR, NV_PRMCIO_SIZE, PRMCIORead, PRMCIOWrite });
    m_MemoryRegions.push_back({ NV_PRAMDAC_ADDR, NV_PRAMDAC_SIZE, PRAMDACRead, PRAMDACWrite });
    m_MemoryRegions.push_back({ NV_PRMDIO_ADDR, NV_PRMDIO_SIZE, PRMDIORead, PRMDIOWrite });
    m_MemoryRegions.push_back({ NV_PRAMIN_ADDR, NV_PRAMIN_SIZE, PRAMINRead, PRAMINWrite });
    m_MemoryRegions.push_back({ NV_USER_ADDR, NV_USER_SIZE, USERRead, USERWrite });
}

const NV2ABlockInfo* NV2ADevice::FindBlock(uint32_t addr) {
    // TODO: use an AVL tree to speed up lookups
    for (auto it = m_MemoryRegions.begin(); it != m_MemoryRegions.end(); ++it) {
        if (addr >= it->offset && addr < it->offset + it->size) {
            return &it[0];
        }
    }

    return nullptr;
}

void NV2ADevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
	log_warning("NV2ADevice::IORead:  Unexpected I/O read!   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    *value = 0;
}

void NV2ADevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
	log_warning("NV2ADevice::IOWrite: Unexpected I/O write!  bar = %d,  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);
}

void NV2ADevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    log_spew("NV2ADevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %u\n", barIndex, addr, size);

    if (barIndex == 1) {
        switch (size) {
        case 1:
            *value = m_VRAM[addr];
            return;
        case 2:
            *value = *(uint16_t*)(&m_VRAM[addr]);
            return;
        case 4:
            *value = *(uint32_t*)(&m_VRAM[addr]);
            return;
        }

        log_warning("NV2ADevice::MMIORead:  Unexpected framebuffer read size!  addr = 0x%x,  size = %u\n", addr, size);
        *value = 0;
        return;
    }

    auto memoryBlock = FindBlock(addr);
    if (memoryBlock != nullptr) {
        memoryBlock->read(this, addr - memoryBlock->offset, value, size);
        return;
    }

	log_warning("NV2ADevice::MMIORead:  Unimplemented!  bar = %d,  addr = 0x%x,  size = %u\n", barIndex, addr, size);

    *value = 0;
}

void NV2ADevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("NV2ADevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %u,  value = 0x%x\n", barIndex, addr, size, value);

    if (barIndex == 1) {
        switch (size) {
        case 1:
            m_VRAM[addr] = value;
            return;
        case 2:
            *(uint16_t*)(&m_VRAM[addr]) = value;
            return;
        case 4:
            *(uint32_t*)(&m_VRAM[addr]) = value;
            return;
        }

        log_warning("NV2ADevice::MMIOWrite: Unimplemented framebuffer write!  addr = 0x%x,  size = %u,  value = 0x%x\n", addr, size, value);
    }

    // Currently we only support 32-bit accesses
    auto memoryBlock = FindBlock(addr);
    if (memoryBlock != nullptr) {
        memoryBlock->write(this, addr - memoryBlock->offset, value, size);
        return;
    }

	log_warning("NV2ADevice::MMIOWrite: Unimplemented!  bar = %d,  addr = 0x%x,  size = %u,  value = 0x%x\n", barIndex, addr, size, value);
}

// ----- NV2A I/O -------------------------------------------------------------

void NV2ADevice::PMCRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    switch (addr) {
    case NV_PMC_BOOT_0:
        // NV2ADevice, A02, Revision 0
        *value = 0x02A000A2;
        return;
    case NV_PMC_INTR_0:
        *value = nv2a->m_PMC.pendingInterrupts;
        return;
    case NV_PMC_INTR_EN_0:
        *value = nv2a->m_PMC.enabledInterrupts;
        return;
    }

    log_warning("NV2ADevice::PMCRead:  Unknown NV2A read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PMCWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    assert(size == 4);

    switch (addr) {
    case NV_PMC_INTR_0:
        nv2a->m_PMC.pendingInterrupts &= ~value;
        nv2a->UpdateIRQ();
        break;
    case NV_PMC_INTR_EN_0:
        nv2a->m_PMC.enabledInterrupts = value;
        nv2a->UpdateIRQ();
        break;
    default:
        log_warning("NV2ADevice::PMCWrite: Unknown NV2A PMC write!  addr = 0x%x,  size = %u,  value = 0x%x\n", addr, size, value);
        break;
    }
}

void NV2ADevice::PBUSRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    assert(size == 4);

    switch (addr) {
    case NV_PBUS_PCI_NV_0:
        *value = PCI_VENDOR_ID_NVIDIA;
        return;

    case NV_PBUS_PCI_NV_1:
        *value = 1; // NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED
        return;

    case NV_PBUS_PCI_NV_2:
        *value = (0x02 << 24) | 161; // PCI_CLASS_DISPLAY_3D (0x02) Rev 161 (0xA1)
        return;
    }

    log_warning("NV2ADevice::PBUSRead:  Unknown NV2A PBUS read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
    return;
}

void NV2ADevice::PBUSWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    switch (addr) {
    case NV_PBUS_PCI_NV_1:
        nv2a->Write16(nv2a->m_configSpace, PCI_COMMAND, value);
        break;
    default:
        break;
    }

    log_warning("NV2ADevice::PBUSWrite: Unknown NV2A PBUS write!  addr = 0x%x,  size = %u,  value = 0x%x\n", addr, size, value);
}

void NV2ADevice::PFIFORead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    // TODO: Acknowledge the size.
    //assert(size == 4);

    switch (addr) {
    case NV_PFIFO_RAMHT:
        *value = 0x03000100; // = NV_PFIFO_RAMHT_SIZE_4K | NV_PFIFO_RAMHT_BASE_ADDRESS(NumberOfPaddingBytes >> 12) | NV_PFIFO_RAMHT_SEARCH_128
        break;
    case NV_PFIFO_RAMFC:
        *value = 0x00890110; // = ? | NV_PFIFO_RAMFC_SIZE_2K | ?
        break;
    case NV_PFIFO_INTR_0:
        *value = nv2a->m_PFIFO.pending_interrupts;
        break;
    case NV_PFIFO_INTR_EN_0:
        *value = nv2a->m_PFIFO.enabled_interrupts;
        break;
    case NV_PFIFO_RUNOUT_STATUS:
        *value = NV_PFIFO_RUNOUT_STATUS_LOW_MARK; /* low mark empty */
        break;
    case NV_PFIFO_CACHE1_PUSH0:
        *value = nv2a->m_PFIFO.cache1.push_enabled;
        break;
    case NV_PFIFO_CACHE1_PUSH1:
        SET_MASK(*value, NV_PFIFO_CACHE1_PUSH1_CHID, nv2a->m_PFIFO.cache1.channel_id);
        SET_MASK(*value, NV_PFIFO_CACHE1_PUSH1_MODE, nv2a->m_PFIFO.cache1.mode);
        break;
    case NV_PFIFO_CACHE1_STATUS:
    {
        std::lock_guard<std::mutex> lk(nv2a->m_PFIFO.cache1.mutex);

        if (nv2a->m_PFIFO.cache1.cache.empty()) {
            *value |= NV_PFIFO_CACHE1_STATUS_LOW_MARK; /* low mark empty */
        }

    }	break;
    case NV_PFIFO_CACHE1_DMA_PUSH:
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS,
            nv2a->m_PFIFO.cache1.dma_push_enabled);
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS,
            nv2a->m_PFIFO.cache1.dma_push_suspended);
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_PUSH_BUFFER, 1); /* buffer emoty */
        break;
    case NV_PFIFO_CACHE1_DMA_STATE:
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE,
            nv2a->m_PFIFO.cache1.method_nonincreasing);
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_STATE_METHOD,
            nv2a->m_PFIFO.cache1.method >> 2);
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL,
            nv2a->m_PFIFO.cache1.subchannel);
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT,
            nv2a->m_PFIFO.cache1.method_count);
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
            nv2a->m_PFIFO.cache1.error);
        break;
    case NV_PFIFO_CACHE1_DMA_INSTANCE:
        SET_MASK(*value, NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS,
            nv2a->m_PFIFO.cache1.dma_instance >> 4);
        break;
    case NV_PFIFO_CACHE1_DMA_PUT:
        *value = nv2a->m_User.channel_control[nv2a->m_PFIFO.cache1.channel_id].dma_put;
        break;
    case NV_PFIFO_CACHE1_DMA_GET:
        *value = nv2a->m_User.channel_control[nv2a->m_PFIFO.cache1.channel_id].dma_get;
        break;
    case NV_PFIFO_CACHE1_DMA_SUBROUTINE:
        *value = nv2a->m_PFIFO.cache1.subroutine_return
            | nv2a->m_PFIFO.cache1.subroutine_active;
        break;
    case NV_PFIFO_CACHE1_PULL0:
    {
        std::lock_guard<std::mutex> lk(nv2a->m_PFIFO.cache1.mutex);
        *value = nv2a->m_PFIFO.cache1.pull_enabled;
    } break;
    case NV_PFIFO_CACHE1_ENGINE:
    {
        std::lock_guard<std::mutex> lk(nv2a->m_PFIFO.cache1.mutex);
        for (int i = 0; i < NV2A_NUM_SUBCHANNELS; i++) {
            *value |= nv2a->m_PFIFO.cache1.bound_engines[i] << (i * 2);
        }

    }	break;
    case NV_PFIFO_CACHE1_DMA_DCOUNT:
        *value = nv2a->m_PFIFO.cache1.dcount;
        break;
    case NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW:
        *value = nv2a->m_PFIFO.cache1.get_jmp_shadow;
        break;
    case NV_PFIFO_CACHE1_DMA_RSVD_SHADOW:
        *value = nv2a->m_PFIFO.cache1.rsvd_shadow;
        break;
    case NV_PFIFO_CACHE1_DMA_DATA_SHADOW:
        *value = nv2a->m_PFIFO.cache1.data_shadow;
        break;
    default:
        *value = nv2a->m_PFIFO.regs[addr];
        break;
    }
}

void NV2ADevice::PFIFOWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    assert(size == 4);

    switch (addr) {
    case NV_PFIFO_INTR_0:
        nv2a->m_PFIFO.pending_interrupts &= ~value;
        nv2a->UpdateIRQ();
        break;
    case NV_PFIFO_INTR_EN_0:
        nv2a->m_PFIFO.enabled_interrupts = value;
        nv2a->UpdateIRQ();
        break;
    case NV_PFIFO_CACHE1_PUSH0:
        nv2a->m_PFIFO.cache1.push_enabled = value & NV_PFIFO_CACHE1_PUSH0_ACCESS;
        break;
    case NV_PFIFO_CACHE1_PUSH1:
        nv2a->m_PFIFO.cache1.channel_id = GET_MASK(value, NV_PFIFO_CACHE1_PUSH1_CHID);
        nv2a->m_PFIFO.cache1.mode = (FifoMode)GET_MASK(value, NV_PFIFO_CACHE1_PUSH1_MODE);
        assert(nv2a->m_PFIFO.cache1.channel_id < NV2A_NUM_CHANNELS);
        break;
    case NV_PFIFO_CACHE1_DMA_PUSH:
        nv2a->m_PFIFO.cache1.dma_push_enabled = GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS);
        if (nv2a->m_PFIFO.cache1.dma_push_suspended && !GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS)) {
            nv2a->m_PFIFO.cache1.dma_push_suspended = false;
            nv2a->pfifo_run_pusher();
        }
        nv2a->m_PFIFO.cache1.dma_push_suspended = GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS);
        break;
    case NV_PFIFO_CACHE1_DMA_STATE:
        nv2a->m_PFIFO.cache1.method_nonincreasing = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE);
        nv2a->m_PFIFO.cache1.method = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD) << 2;
        nv2a->m_PFIFO.cache1.subchannel = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL);
        nv2a->m_PFIFO.cache1.method_count = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT);
        nv2a->m_PFIFO.cache1.error = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_ERROR);
        break;
    case NV_PFIFO_CACHE1_DMA_INSTANCE:
        nv2a->m_PFIFO.cache1.dma_instance = GET_MASK(value, NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS) << 4;
        break;
    case NV_PFIFO_CACHE1_DMA_PUT:
        nv2a->m_User.channel_control[nv2a->m_PFIFO.cache1.channel_id].dma_put = value;
        break;
    case NV_PFIFO_CACHE1_DMA_GET:
        nv2a->m_User.channel_control[nv2a->m_PFIFO.cache1.channel_id].dma_get = value;
        break;
    case NV_PFIFO_CACHE1_DMA_SUBROUTINE:
        nv2a->m_PFIFO.cache1.subroutine_return = (value & NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET);
        nv2a->m_PFIFO.cache1.subroutine_active = (value & NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE);
        break;
    case NV_PFIFO_CACHE1_PULL0:
    {
        std::lock_guard<std::mutex> lk(nv2a->m_PFIFO.cache1.mutex);

        if ((value & NV_PFIFO_CACHE1_PULL0_ACCESS)
            && !nv2a->m_PFIFO.cache1.pull_enabled) {
            nv2a->m_PFIFO.cache1.pull_enabled = true;

            /* the puller thread should wake up */
            nv2a->m_PFIFO.cache1.cache_cond.notify_all();
        }
        else if (!(value & NV_PFIFO_CACHE1_PULL0_ACCESS)
            && nv2a->m_PFIFO.cache1.pull_enabled) {
            nv2a->m_PFIFO.cache1.pull_enabled = false;
        }
    }	break;
    case NV_PFIFO_CACHE1_ENGINE:
    {
        std::lock_guard<std::mutex> lk(nv2a->m_PFIFO.cache1.mutex);

        for (int i = 0; i < NV2A_NUM_SUBCHANNELS; i++) {
            nv2a->m_PFIFO.cache1.bound_engines[i] = (FIFOEngine)((value >> (i * 2)) & 3);
        }

    } break;
    case NV_PFIFO_CACHE1_DMA_DCOUNT:
        nv2a->m_PFIFO.cache1.dcount = (value & NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE);
        break;
    case NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW:
        nv2a->m_PFIFO.cache1.get_jmp_shadow = (value & NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
        break;
    case NV_PFIFO_CACHE1_DMA_RSVD_SHADOW:
        nv2a->m_PFIFO.cache1.rsvd_shadow = value;
        break;
    case NV_PFIFO_CACHE1_DMA_DATA_SHADOW:
        nv2a->m_PFIFO.cache1.data_shadow = value;
        break;
    default:
        nv2a->m_PFIFO.regs[addr] = value;
        break;
    }
}

void NV2ADevice::PRMARead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PRMARead:  Unknown NV2A PRMA read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PRMAWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PRMAWrite: Unknown NV2A PRMA write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PVIDEORead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    switch (addr) {
    case NV_PVIDEO_STOP:
        *value = 0;
        break;

    default:
        *value = nv2a->m_PVIDEO.regs[addr];
        break;
    }
}

void NV2ADevice::PVIDEOWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    switch (addr) {
    case NV_PVIDEO_BUFFER:
        nv2a->m_PVIDEO.regs[addr] = value;
        break;

    case NV_PVIDEO_STOP:
        nv2a->m_PVIDEO.regs[NV_PVIDEO_BUFFER] = 0;
        break;

    default:
        nv2a->m_PVIDEO.regs[addr] = addr;
    }
}

static inline uint64_t muldiv64(uint64_t a, uint32_t b, uint32_t c) {
    union {
        uint64_t ll;
        struct {
            uint32_t low, high;
        } l;
    } u, res;
    uint64_t rl, rh;

    u.ll = a;
    rl = (uint64_t)u.l.low * (uint64_t)b;
    rh = (uint64_t)u.l.high * (uint64_t)b;
    rh += (rl >> 32);
    res.l.high = rh / c;
    res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
    return res.ll;
}

uint32_t NV2ADevice::ptimer_get_clock() {
    // Get time in nanoseconds
    long int time = static_cast<long int>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    return muldiv64(time, m_PRAMDAC.core_clock_freq * m_PTIMER.numerator, CLOCKS_PER_SEC * m_PTIMER.denominator);
}

void NV2ADevice::PTIMERRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    switch (addr) {
    case NV_PTIMER_INTR_0:
        *value = nv2a->m_PTIMER.pending_interrupts;
        break;
    case NV_PTIMER_INTR_EN_0:
        *value = nv2a->m_PTIMER.enabled_interrupts;
        break;
    case NV_PTIMER_NUMERATOR:
        *value = nv2a->m_PTIMER.numerator;
        break;
    case NV_PTIMER_DENOMINATOR:
        *value = nv2a->m_PTIMER.denominator;
        break;
    case NV_PTIMER_TIME_0:
        *value = (nv2a->ptimer_get_clock() & 0x7ffffff) << 5;
        break;
    case NV_PTIMER_TIME_1:
        *value = (nv2a->ptimer_get_clock() >> 27) & 0x1fffffff;
        break;
    default:
        *value = 0;
        break;
    }
}

void NV2ADevice::PTIMERWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    switch (addr) {
    case NV_PTIMER_INTR_0:
        nv2a->m_PTIMER.pending_interrupts &= ~value;
        nv2a->UpdateIRQ();
        break;
    case NV_PTIMER_INTR_EN_0:
        nv2a->m_PTIMER.enabled_interrupts = value;
        nv2a->UpdateIRQ();
        break;
    case NV_PTIMER_DENOMINATOR:
        nv2a->m_PTIMER.denominator = value;
        break;
    case NV_PTIMER_NUMERATOR:
        nv2a->m_PTIMER.numerator = value;
        break;
    case NV_PTIMER_ALARM_0:
        nv2a->m_PTIMER.alarm_time = value;
        break;
    default:
        break;
    }
}

void NV2ADevice::PCOUNTERRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PCOUNTERRead:  Unknown NV2A PCOUNTER read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PCOUNTERWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PCOUNTERWrite: Unknown NV2A PCOUNTER write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PVPERead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PVPERead:  Unknown NV2A PVPE read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PVPEWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PVPEWrite: Unknown NV2A PVPE write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PTVRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PTVRead:  Unknown NV2A PTV read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PTVWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PTVWrite: Unknown NV2A PTV write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PRMFBRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PRMFBRead:  Unknown NV2A PRMFB read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PRMFBWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PRMFBWrite: Unknown NV2A PRMFB write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PRMVIORead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PRMVIORead:  Unknown NV2A PRMVIO read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PRMVIOWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PRMVIOWrite: Unknown NV2A PRMVIO write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PFBRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    assert(size == 4);

    switch (addr) {
    case NV_PFB_CFG0:
        *value = 3; // 3 memory partitions
        break;
    case NV_PFB_CSTATUS:
        *value = nv2a->m_systemRAMSize;
        break;
    case NV_PFB_WBC:
        *value = 0;
        break;
    default:
        *value = nv2a->m_PFB.registers[addr];
        break;
    }
}

void NV2ADevice::PFBWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    assert(size == 4);
    nv2a->m_PFB.registers[addr] = value;
}

void NV2ADevice::PSTRAPSRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PSTRAPSRead:  Unknown NV2A PSTRAPS read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PSTRAPSWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PSTRAPSWrite: Unknown NV2A PSTRAPS write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PGRAPHRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    assert(size == 4);

    std::lock_guard<std::mutex> lk(nv2a->m_PGRAPH.mutex);

    switch (addr) {
    case NV_PGRAPH_INTR:
        *value = nv2a->m_PGRAPH.pending_interrupts;
        break;

    case NV_PGRAPH_INTR_EN:
        *value = nv2a->m_PGRAPH.enabled_interrupts;
        break;

    case NV_PGRAPH_NSOURCE:
        *value = nv2a->m_PGRAPH.notify_source;
        break;

    case NV_PGRAPH_CTX_USER:
        SET_MASK(*value, NV_PGRAPH_CTX_USER_CHANNEL_3D, nv2a->m_PGRAPH.context[nv2a->m_PGRAPH.channel_id].channel_3d);
        SET_MASK(*value, NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID, 1);
        SET_MASK(*value, NV_PGRAPH_CTX_USER_SUBCH, nv2a->m_PGRAPH.context[nv2a->m_PGRAPH.channel_id].subchannel << 13);
        SET_MASK(*value, NV_PGRAPH_CTX_USER_CHID, nv2a->m_PGRAPH.channel_id);
        break;

    case NV_PGRAPH_TRAPPED_ADDR:
        SET_MASK(*value, NV_PGRAPH_TRAPPED_ADDR_CHID, nv2a->m_PGRAPH.trapped_channel_id);
        SET_MASK(*value, NV_PGRAPH_TRAPPED_ADDR_SUBCH, nv2a->m_PGRAPH.trapped_subchannel);
        SET_MASK(*value, NV_PGRAPH_TRAPPED_ADDR_MTHD, nv2a->m_PGRAPH.trapped_method);
        break;

    case NV_PGRAPH_TRAPPED_DATA_LOW:
        *value = nv2a->m_PGRAPH.trapped_data[0];
        break;

    case NV_PGRAPH_FIFO:
        SET_MASK(*value, NV_PGRAPH_FIFO_ACCESS, nv2a->m_PGRAPH.fifo_access);
        break;

    case NV_PGRAPH_CHANNEL_CTX_TABLE:
        *value = nv2a->m_PGRAPH.context_table >> 4;
        break;

    case NV_PGRAPH_CHANNEL_CTX_POINTER:
        *value = nv2a->m_PGRAPH.context_address >> 4;
        break;

    default:
        *value = nv2a->m_PGRAPH.regs[addr];
        break;
    }
}

void NV2ADevice::PGRAPHWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    std::lock_guard<std::mutex> lk(nv2a->m_PGRAPH.mutex);

    switch (addr) {
    case NV_PGRAPH_INTR:
        nv2a->m_PGRAPH.pending_interrupts &= ~value;
        nv2a->m_PGRAPH.interrupt_cond.notify_all();
        break;
    case NV_PGRAPH_INTR_EN:
        nv2a->m_PGRAPH.enabled_interrupts = value;
        break;
    case NV_PGRAPH_CTX_CONTROL:
        nv2a->m_PGRAPH.channel_valid = (value & NV_PGRAPH_CTX_CONTROL_CHID);
        break;
    case NV_PGRAPH_CTX_USER:
        nv2a->pgraph_set_context_user(value);
        break;
    case NV_PGRAPH_INCREMENT:
        if (value & NV_PGRAPH_INCREMENT_READ_3D) {
            SET_MASK(nv2a->m_PGRAPH.regs[NV_PGRAPH_SURFACE],
                NV_PGRAPH_SURFACE_READ_3D,
                (GET_MASK(nv2a->m_PGRAPH.regs[NV_PGRAPH_SURFACE],
                    NV_PGRAPH_SURFACE_READ_3D) + 1)
                % GET_MASK(nv2a->m_PGRAPH.regs[NV_PGRAPH_SURFACE],
                    NV_PGRAPH_SURFACE_MODULO_3D));

            nv2a->m_PGRAPH.flip_3d.notify_all();
        }
        break;
    case NV_PGRAPH_FIFO:
        nv2a->m_PGRAPH.fifo_access = GET_MASK(value, NV_PGRAPH_FIFO_ACCESS);
        nv2a->m_PGRAPH.fifo_access_cond.notify_all();
        break;
    case NV_PGRAPH_CHANNEL_CTX_TABLE:
        nv2a->m_PGRAPH.context_table = (value & NV_PGRAPH_CHANNEL_CTX_TABLE_INST) << 4;
        break;
    case NV_PGRAPH_CHANNEL_CTX_POINTER:
        nv2a->m_PGRAPH.context_address =
            (value & NV_PGRAPH_CHANNEL_CTX_POINTER_INST) << 4;
        break;
    case NV_PGRAPH_CHANNEL_CTX_TRIGGER:
        if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN) {
            log_debug("PGRAPH: read channel %d context from %0x08X\n",
                nv2a->m_PGRAPH.channel_id, nv2a->m_PGRAPH.context_address);

            uint8_t *context_ptr = (uint8_t*)(nv2a->m_pRAMIN + nv2a->m_PGRAPH.context_address);
            uint32_t context_user = ldl_le_p((uint32_t*)context_ptr);

            log_debug("    - CTX_USER = 0x%x\n", context_user);

            //pgraph_set_context_user
            nv2a->m_PGRAPH.channel_id = (context_user & NV_PGRAPH_CTX_USER_CHID) >> 24;
            nv2a->m_PGRAPH.context[nv2a->m_PGRAPH.channel_id].channel_3d = GET_MASK(context_user, NV_PGRAPH_CTX_USER_CHANNEL_3D);
            nv2a->m_PGRAPH.context[nv2a->m_PGRAPH.channel_id].subchannel = GET_MASK(context_user, NV_PGRAPH_CTX_USER_SUBCH);

        }
        if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_WRITE_OUT) {
            /* do stuff ... */
        }

        break;
    default:
        nv2a->m_PGRAPH.regs[addr] = value;
        break;
    }
}

void NV2ADevice::PCRTCRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    switch (addr) {
    case NV_PCRTC_INTR_0:
        *value = nv2a->m_PCRTC.pendingInterrupts;
        break;
    case NV_PCRTC_INTR_EN_0:
        *value = nv2a->m_PCRTC.enabledInterrupts;
        break;
    case NV_PCRTC_START:
        *value = nv2a->m_PCRTC.start;
        break;
    default:
        *value = 0;
        break;
    }
}

void NV2ADevice::PCRTCWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    switch (addr) {
    case NV_PCRTC_INTR_0:
        nv2a->m_PCRTC.pendingInterrupts &= ~value;
        nv2a->UpdateIRQ();
        break;
    case NV_PCRTC_INTR_EN_0:
        nv2a->m_PCRTC.enabledInterrupts = value;
        nv2a->UpdateIRQ();
        break;
    case NV_PCRTC_START:
        nv2a->m_PCRTC.start = value &= 0x07FFFFFF;
        break;
    default:
        nv2a->m_PCRTC.regs[addr] = value;
        break;
    }

}

void NV2ADevice::PRMCIORead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    switch (addr) {
    case VGA_CRT_IM:
    case VGA_CRT_IC:
        *value = nv2a->m_PRMCIO.cr_index;
        break;

    case VGA_IS1_RC:
    case VGA_IS1_RM:
        // Toggle the retrace bit to fool polling. QEMU does the same.
        *value = nv2a->m_VGAState.st01;
        nv2a->m_VGAState.st01 ^= 1 << 3;
        break;

    case VGA_CRT_DM:
    case VGA_CRT_DC:
        *value = nv2a->m_PRMCIO.cr[nv2a->m_PRMCIO.cr_index];

        log_debug("vga: read CR%x = 0x%02x\n", nv2a->m_PRMCIO.cr_index, *value);
        break;
    default:
        log_warning("NV2ADevice::PRMCIORead:  Unknown NV2A PRMCIO read!   addr = 0x%x,  size = %u\n", addr, size);
        break;
    }
}

void NV2ADevice::PRMCIOWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    switch (addr) {
    case VGA_CRT_IM:
    case VGA_CRT_IC:
        nv2a->m_PRMCIO.cr_index = value;
        break;
    case VGA_CRT_DM:
    case VGA_CRT_DC:
        log_debug("vga: write CR%x = 0x%02x\n", nv2a->m_PRMCIO.cr_index, value);

        /* handle CR0-7 protection */
        if ((nv2a->m_PRMCIO.cr[VGA_CRTC_V_SYNC_END] & VGA_CR11_LOCK_CR0_CR7) &&
            nv2a->m_PRMCIO.cr_index <= VGA_CRTC_OVERFLOW) {
            /* can always write bit 4 of CR7 */
            if (nv2a->m_PRMCIO.cr_index == VGA_CRTC_OVERFLOW) {
                nv2a->m_PRMCIO.cr[VGA_CRTC_OVERFLOW] = (nv2a->m_PRMCIO.cr[VGA_CRTC_OVERFLOW] & ~0x10) |
                    (value & 0x10);
                log_debug("TODO: vbe_update_vgaregs\n");
                //vbe_update_vgaregs();
            }
            return;
        }

        nv2a->m_PRMCIO.cr[nv2a->m_PRMCIO.cr_index] = value;
        log_debug("TODO: vbe_update_vgaregs\n");
        //vbe_update_vgaregs();

        switch (nv2a->m_PRMCIO.cr_index) {
        case VGA_CRTC_H_TOTAL:
        case VGA_CRTC_H_SYNC_START:
        case VGA_CRTC_H_SYNC_END:
        case VGA_CRTC_V_TOTAL:
        case VGA_CRTC_OVERFLOW:
        case VGA_CRTC_V_SYNC_END:
        case VGA_CRTC_MODE:
            // TODO: s->update_retrace_info(s);
            log_debug("TODO: update_retrace_info\n");
            break;
        }
        break;
    default:
        log_warning("NV2ADevice::PRMCIOWrite: Unknown NV2A PRMCIO write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
        break;
    }
}

void NV2ADevice::PRAMDACRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    switch (addr) {
    case NV_PRAMDAC_NVPLL_COEFF:
        *value = nv2a->m_PRAMDAC.core_clock_coeff;
        break;
    case NV_PRAMDAC_MPLL_COEFF:
        *value = nv2a->m_PRAMDAC.memory_clock_coeff;
        break;
    case NV_PRAMDAC_VPLL_COEFF:
        *value = nv2a->m_PRAMDAC.video_clock_coeff;
        break;
    case NV_PRAMDAC_PLL_TEST_COUNTER:
        /* emulated PLLs locked instantly? */
        *value = NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK
            | NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK
            | NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK
            | NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK;
        break;
    default:
        log_warning("NV2ADevice::PRAMDACRead:  Unknown NV2A PRAMDAC read!   addr = 0x%x,  size = %u\n", addr, size);
        *value = 0;
        break;
    }
}

void NV2ADevice::PRAMDACWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    switch (addr) {
    case NV_PRAMDAC_NVPLL_COEFF:
        uint32_t m, n, p;
        nv2a->m_PRAMDAC.core_clock_coeff = value;

        m = value & NV_PRAMDAC_NVPLL_COEFF_MDIV;
        n = (value & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
        p = (value & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

        if (m == 0) {
            nv2a->m_PRAMDAC.core_clock_freq = 0;
        }
        else {
            nv2a->m_PRAMDAC.core_clock_freq = (NV2A_CRYSTAL_FREQ * n) / (1 << p) / m;
        }

        break;
    case NV_PRAMDAC_MPLL_COEFF:
        nv2a->m_PRAMDAC.memory_clock_coeff = value;
        break;
    case NV_PRAMDAC_VPLL_COEFF:
        nv2a->m_PRAMDAC.video_clock_coeff = value;
        break;

    default:
        log_warning("NV2ADevice::PRAMDACWrite: Unknown NV2A PRAMDAC write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
        break;
    }
}

void NV2ADevice::PRMDIORead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PRMDIORead:  Unknown NV2A PRMDIO read!   addr = 0x%x,  size = %u\n", addr, size);
    *value = 0;
}

void NV2ADevice::PRMDIOWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PRMDIOWrite: Unknown NV2A PRMDIO write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
}

void NV2ADevice::PRAMINRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    void* ptr = (uint8_t*)nv2a->m_pRAMIN + addr;

    switch (size) {
    case 1:
        *value = *((uint8_t*)ptr);
        break;
    case 2:
        *value = *((uint16_t*)ptr);
        break;
    case 4:
        *value = *((uint32_t*)ptr);
        break;
    default:
        *value = 0;
        break;
    }
}

void NV2ADevice::PRAMINWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    void* ptr = (uint8_t*)nv2a->m_pRAMIN + addr;

    switch (size) {
    case 1:
        *((uint8_t*)ptr) = value;
        break;
    case 2:
        *((uint16_t*)ptr) = value;
        break;
    case 4:
        *((uint32_t*)ptr) = value;
        break;
    }
}

void NV2ADevice::USERRead(NV2ADevice *nv2a, uint32_t addr, uint32_t *value, uint8_t size) {
    assert(size == 4);

    unsigned int channel_id = addr >> 16;
    assert(channel_id < NV2A_NUM_CHANNELS);

    ChannelControl *control = &nv2a->m_User.channel_control[channel_id];
    uint32_t channel_modes = nv2a->m_PFIFO.regs[NV_PFIFO_MODE];

    /* PIO Mode */
    if (!(channel_modes & (1 << channel_id))) {
        assert(false);
    }

    /* DMA Mode */
    addr &= 0xFFFF;
    switch (addr) {
    case NV_USER_DMA_PUT:
        *value = control->dma_put;
        break;
    case NV_USER_DMA_GET:
        *value = control->dma_get;
        break;
    case NV_USER_REF:
        *value = control->ref;
        break;
    default:
        log_warning("NV2ADevice::USERRead:  Unknown NV2A USER read!   addr = 0x%x,  size = %u\n", addr, size);
        break;
    }
}

void NV2ADevice::USERWrite(NV2ADevice *nv2a, uint32_t addr, uint32_t value, uint8_t size) {
    assert(size == 4);

    unsigned int channel_id = addr >> 16;
    assert(channel_id < NV2A_NUM_CHANNELS);

    ChannelControl *control = &nv2a->m_User.channel_control[channel_id];

    uint32_t channel_modes = nv2a->m_PFIFO.regs[NV_PFIFO_MODE];

    if (channel_modes & (1 << channel_id)) {
        /* DMA Mode */
        switch (addr & 0xFFFF) {
        case NV_USER_DMA_PUT:
            control->dma_put = value;

            if (nv2a->m_PFIFO.cache1.push_enabled) {
                nv2a->pfifo_run_pusher();
            }
            break;
        case NV_USER_DMA_GET:
            control->dma_get = value;
            break;
        case NV_USER_REF:
            control->ref = value;
            break;
        default:
            log_warning("NV2ADevice::USERWrite: Unknown NV2A USER write!  addr = 0x%x,  size: %d,  value: 0x%x\n", addr, size, value);
            break;
        }
    }
    else {
        /* PIO Mode */
        assert(false);
    }
}

// ----- Utility functions ----------------------------------------------------

uint32_t NV2ADevice::ramht_hash(uint32_t handle) {
    unsigned int ramht_size = 1 << (GET_MASK(m_PFIFO.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE) + 12);

    /* XXX: Think this is different to what nouveau calculates... */
    unsigned int bits = ffs(ramht_size) - 2;

    uint32_t hash = 0;
    while (handle) {
        hash ^= (handle & ((1 << bits) - 1));
        handle >>= bits;
    }
    hash ^= m_PFIFO.cache1.channel_id << (bits - 4);

    return hash;
}

RAMHTEntry NV2ADevice::ramht_lookup(uint32_t handle) {
    unsigned int ramht_size = 1 << (GET_MASK(m_PFIFO.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE) + 12);

    uint32_t hash = ramht_hash(handle);
    assert(hash * 8 < ramht_size);

    uint32_t ramht_address =
        GET_MASK(m_PFIFO.regs[NV_PFIFO_RAMHT],
            NV_PFIFO_RAMHT_BASE_ADDRESS) << 12;

    uint8_t *entry_ptr = (uint8_t*)(m_pRAMIN + ramht_address + hash * 8);

    uint32_t entry_handle = ldl_le_p((uint32_t*)entry_ptr);
    uint32_t entry_context = ldl_le_p((uint32_t*)(entry_ptr + 4));

    RAMHTEntry entry;
    entry.handle = entry_handle;
    entry.instance = (entry_context & NV_RAMHT_INSTANCE) << 4;
    entry.engine = (FIFOEngine)((entry_context & NV_RAMHT_ENGINE) >> 16);
    entry.channel_id = (entry_context & NV_RAMHT_CHID) >> 24;
    entry.valid = entry_context & NV_RAMHT_STATUS;

    return entry;
}

void NV2ADevice::pgraph_set_context_user(uint32_t value) {
    m_PGRAPH.channel_id = (value & NV_PGRAPH_CTX_USER_CHID) >> 24;
    m_PGRAPH.context[m_PGRAPH.channel_id].channel_3d = GET_MASK(value, NV_PGRAPH_CTX_USER_CHANNEL_3D);
    m_PGRAPH.context[m_PGRAPH.channel_id].subchannel = GET_MASK(value, NV_PGRAPH_CTX_USER_SUBCH);
}

void NV2ADevice::pgraph_context_switch(unsigned int channel_id) {
    bool valid = false;

    // Scope the lock so that it gets unlocked at end of this block
    {
        std::lock_guard<std::mutex> lk(m_PGRAPH.mutex);

        valid = m_PGRAPH.channel_valid && m_PGRAPH.channel_id == channel_id;
        if (!valid) {
            m_PGRAPH.trapped_channel_id = channel_id;
        }
    }

    if (!valid) {
        log_debug("puller needs to switch to ch %d\n", channel_id);

        //qemu_mutex_lock_iothread();
        m_PGRAPH.pending_interrupts |= NV_PGRAPH_INTR_CONTEXT_SWITCH;
        UpdateIRQ();

        std::unique_lock<std::mutex> lk(m_PGRAPH.mutex);
        //qemu_mutex_unlock_iothread();

        while (m_PGRAPH.pending_interrupts & NV_PGRAPH_INTR_CONTEXT_SWITCH) {
            m_PGRAPH.interrupt_cond.wait(lk);
        }
    }
}

void NV2ADevice::pgraph_wait_fifo_access() {
    std::unique_lock<std::mutex> lk(m_PGRAPH.mutex);

    while (!m_PGRAPH.fifo_access) {
        m_PGRAPH.fifo_access_cond.wait(lk);
    }
}

void NV2ADevice::load_graphics_object(uint32_t instance_address, GraphicsObject *obj) {
    uint8_t *obj_ptr;
    uint32_t switch1, switch2, switch3;

    assert(instance_address < NV_PRAMIN_SIZE);
    obj_ptr = (uint8_t*)(m_pRAMIN + instance_address);

    switch1 = ldl_le_p((uint32_t*)obj_ptr);
    switch2 = ldl_le_p((uint32_t*)(obj_ptr + 4));
    switch3 = ldl_le_p((uint32_t*)(obj_ptr + 8));

    obj->graphics_class = switch1 & NV_PGRAPH_CTX_SWITCH1_GRCLASS;

    /* init graphics object */
    switch (obj->graphics_class) {
    case NV_KELVIN_PRIMITIVE:
        // kelvin->vertex_attributes[NV2A_VERTEX_ATTR_DIFFUSE].inline_value = 0xFFFFFFF;
        break;
    default:
        break;
    }
}

GraphicsObject* NV2ADevice::lookup_graphics_object(uint32_t instance_address) {
    int i;
    for (i = 0; i<NV2A_NUM_SUBCHANNELS; i++) {
        if (m_PGRAPH.subchannel_data[i].object_instance == instance_address) {
            return &m_PGRAPH.subchannel_data[i].object;
        }
    }
    return NULL;
}

DMAObject NV2ADevice::nv_dma_load(uint32_t dma_obj_address) {
    assert(dma_obj_address < NV_PRAMIN_SIZE);

    uint32_t *dma_obj = (uint32_t*)(m_pRAMIN + dma_obj_address);
    uint32_t flags = ldl_le_p(dma_obj);
    uint32_t limit = ldl_le_p(dma_obj + 1);
    uint32_t frame = ldl_le_p(dma_obj + 2);

    DMAObject object;
    object.dma_class = GET_MASK(flags, NV_DMA_CLASS);
    object.dma_target = GET_MASK(flags, NV_DMA_TARGET);
    object.address = (frame & NV_DMA_ADDRESS) | GET_MASK(flags, NV_DMA_ADJUST);
    object.limit = limit;

    return object;
}

void *NV2ADevice::nv_dma_map(uint32_t dma_obj_address, uint32_t *len) {
    assert(dma_obj_address < NV_PRAMIN_SIZE);

    DMAObject dma = nv_dma_load(dma_obj_address);

    /* TODO: Handle targets and classes properly */
    log_debug("dma_map %x, %x, %x %x"  "\n",
        dma.dma_class, dma.dma_target, dma.address, dma.limit);

    dma.address &= 0x07FFFFFF;

    // assert(dma.address + dma.limit < memory_region_size(d->vram));
    *len = dma.limit;
    return (void*)(m_VRAM + dma.address);
}

bool NV2ADevice::pgraph_color_write_enabled() {
    return m_PGRAPH.regs[NV_PGRAPH_CONTROL_0] & (
        NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE
        | NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE
        | NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE
        | NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE);
}

bool NV2ADevice::pgraph_zeta_write_enabled() {
    return m_PGRAPH.regs[NV_PGRAPH_CONTROL_0] & (
        NV_PGRAPH_CONTROL_0_ZWRITEENABLE
        | NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE);
}

unsigned int NV2ADevice::kelvin_map_stencil_op(uint32_t parameter) {
    unsigned int op;
    switch (parameter) {
    case NV097_SET_STENCIL_OP_V_KEEP:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_KEEP; break;
    case NV097_SET_STENCIL_OP_V_ZERO:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_ZERO; break;
    case NV097_SET_STENCIL_OP_V_REPLACE:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_REPLACE; break;
    case NV097_SET_STENCIL_OP_V_INCRSAT:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCRSAT; break;
    case NV097_SET_STENCIL_OP_V_DECRSAT:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECRSAT; break;
    case NV097_SET_STENCIL_OP_V_INVERT:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INVERT; break;
    case NV097_SET_STENCIL_OP_V_INCR:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCR; break;
    case NV097_SET_STENCIL_OP_V_DECR:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECR; break;
    default:
        assert(false);
        break;
    }
    return op;
}

unsigned int NV2ADevice::kelvin_map_polygon_mode(uint32_t parameter) {
    unsigned int mode;
    switch (parameter) {
    case NV097_SET_FRONT_POLYGON_MODE_V_POINT:
        mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_POINT; break;
    case NV097_SET_FRONT_POLYGON_MODE_V_LINE:
        mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_LINE; break;
    case NV097_SET_FRONT_POLYGON_MODE_V_FILL:
        mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_FILL; break;
    default:
        assert(false);
        break;
    }
    return mode;
}

unsigned int NV2ADevice::kelvin_map_texgen(uint32_t parameter, unsigned int channel) {
    assert(channel < 4);
    unsigned int texgen;
    switch (parameter) {
    case NV097_SET_TEXGEN_S_DISABLE:
        texgen = NV_PGRAPH_CSV1_A_T0_S_DISABLE; break;
    case NV097_SET_TEXGEN_S_EYE_LINEAR:
        texgen = NV_PGRAPH_CSV1_A_T0_S_EYE_LINEAR; break;
    case NV097_SET_TEXGEN_S_OBJECT_LINEAR:
        texgen = NV_PGRAPH_CSV1_A_T0_S_OBJECT_LINEAR; break;
    case NV097_SET_TEXGEN_S_SPHERE_MAP:
        assert(channel < 2);
        texgen = NV_PGRAPH_CSV1_A_T0_S_SPHERE_MAP; break;
    case NV097_SET_TEXGEN_S_REFLECTION_MAP:
        assert(channel < 3);
        texgen = NV_PGRAPH_CSV1_A_T0_S_REFLECTION_MAP; break;
    case NV097_SET_TEXGEN_S_NORMAL_MAP:
        assert(channel < 3);
        texgen = NV_PGRAPH_CSV1_A_T0_S_NORMAL_MAP; break;
    default:
        assert(false);
        break;
    }
    return texgen;
}

void NV2ADevice::pgraph_method_log(unsigned int subchannel, unsigned int graphics_class, unsigned int method, uint32_t parameter) {
    static unsigned int last = 0;
    static unsigned int count = 0;

    if (last == 0x1800 && method != last) {
        log_debug("pgraph method (%d) 0x%08X * %d", subchannel, last, count);
    }
    if (method != 0x1800) {
        const char* method_name = NULL;
        unsigned int nmethod = 0;
        switch (graphics_class) {
        case NV_KELVIN_PRIMITIVE:
            nmethod = method | (0x5c << 16);
            break;
        case NV_CONTEXT_SURFACES_2D:
            nmethod = method | (0x6d << 16);
            break;
        default:
            break;
        }
        /*
        if (nmethod != 0 && nmethod < ARRAY_SIZE(nv2a_method_names)) {
        method_name = nv2a_method_names[nmethod];
        }
        if (method_name) {
        log_debug("pgraph method (%d): %s (0x%x)\n",
        subchannel, method_name, parameter);
        }
        else {
        */
        log_debug("pgraph method (%d): 0x%x -> 0x%04x (0x%x)\n",
            subchannel, graphics_class, method, parameter);
        //}

    }
    if (method == last) { count++; }
    else { count = 0; }
    last = method;
}

void NV2ADevice::pgraph_method(unsigned int subchannel, unsigned int method, uint32_t parameter) {
    std::lock_guard<std::mutex> lk(m_PGRAPH.mutex);

    int i;
    GraphicsSubchannel *subchannel_data;
    GraphicsObject *object;

    unsigned int slot;

    assert(m_PGRAPH.channel_valid);
    subchannel_data = &m_PGRAPH.subchannel_data[subchannel];
    object = &subchannel_data->object;

    ContextSurfaces2DState *context_surfaces_2d = &object->data.context_surfaces_2d;
    ImageBlitState *image_blit = &object->data.image_blit;
    KelvinState *kelvin = &object->data.kelvin;

    pgraph_method_log(subchannel, object->graphics_class, method, parameter);

    if (method == NV_SET_OBJECT) {
        subchannel_data->object_instance = parameter;

        //qemu_mutex_lock_iothread();
        load_graphics_object(parameter, object);
        //qemu_mutex_unlock_iothread();
        return;
    }

    switch (object->graphics_class) {
    case NV_CONTEXT_SURFACES_2D:
    {
        switch (method) {
        case NV062_SET_CONTEXT_DMA_IMAGE_SOURCE:
            context_surfaces_2d->dma_image_source = parameter;
            break;
        case NV062_SET_CONTEXT_DMA_IMAGE_DESTIN:
            context_surfaces_2d->dma_image_dest = parameter;
            break;
        case NV062_SET_COLOR_FORMAT:
            context_surfaces_2d->color_format = parameter;
            break;
        case NV062_SET_PITCH:
            context_surfaces_2d->source_pitch = parameter & 0xFFFF;
            context_surfaces_2d->dest_pitch = parameter >> 16;
            break;
        case NV062_SET_OFFSET_SOURCE:
            context_surfaces_2d->source_offset = parameter & 0x07FFFFFF;
            break;
        case NV062_SET_OFFSET_DESTIN:
            context_surfaces_2d->dest_offset = parameter & 0x07FFFFFF;
            break;
        default:
            log_warning("EmuNV2A: Unknown NV_CONTEXT_SURFACES_2D Method: 0x%08X\n", method);
        }

        break;
    }

    case NV_IMAGE_BLIT:
    {
        switch (method) {
        case NV09F_SET_CONTEXT_SURFACES:
            image_blit->context_surfaces = parameter;
            break;
        case NV09F_SET_OPERATION:
            image_blit->operation = parameter;
            break;
        case NV09F_CONTROL_POINT_IN:
            image_blit->in_x = parameter & 0xFFFF;
            image_blit->in_y = parameter >> 16;
            break;
        case NV09F_CONTROL_POINT_OUT:
            image_blit->out_x = parameter & 0xFFFF;
            image_blit->out_y = parameter >> 16;
            break;
        case NV09F_SIZE:
            image_blit->width = parameter & 0xFFFF;
            image_blit->height = parameter >> 16;

            /* I guess this kicks it off? */
            if (image_blit->operation == NV09F_SET_OPERATION_SRCCOPY) {

                log_debug("NV09F_SET_OPERATION_SRCCOPY");

                GraphicsObject *context_surfaces_obj = lookup_graphics_object(image_blit->context_surfaces);
                assert(context_surfaces_obj);
                assert(context_surfaces_obj->graphics_class == NV_CONTEXT_SURFACES_2D);

                ContextSurfaces2DState *context_surfaces = &context_surfaces_obj->data.context_surfaces_2d;

                unsigned int bytes_per_pixel;
                switch (context_surfaces->color_format) {
                case NV062_SET_COLOR_FORMAT_LE_Y8:
                    bytes_per_pixel = 1;
                    break;
                case NV062_SET_COLOR_FORMAT_LE_R5G6B5:
                    bytes_per_pixel = 2;
                    break;
                case NV062_SET_COLOR_FORMAT_LE_A8R8G8B8:
                    bytes_per_pixel = 4;
                    break;
                default:
                    log_debug("Unknown blit surface format: 0x%x\n", context_surfaces->color_format);
                    assert(false);
                    break;
                }

                uint32_t source_dma_len, dest_dma_len;
                uint8_t *source, *dest;

                source = (uint8_t*)nv_dma_map(context_surfaces->dma_image_source, &source_dma_len);
                assert(context_surfaces->source_offset < source_dma_len);
                source += context_surfaces->source_offset;

                dest = (uint8_t*)nv_dma_map(context_surfaces->dma_image_dest, &dest_dma_len);
                assert(context_surfaces->dest_offset < dest_dma_len);
                dest += context_surfaces->dest_offset;

                log_debug("  - 0x%tx -> 0x%tx\n", source - m_VRAM, dest - m_VRAM);

                int y;
                for (y = 0; y<image_blit->height; y++) {
                    uint8_t *source_row = source
                        + (image_blit->in_y + y) * context_surfaces->source_pitch
                        + image_blit->in_x * bytes_per_pixel;

                    uint8_t *dest_row = dest
                        + (image_blit->out_y + y) * context_surfaces->dest_pitch
                        + image_blit->out_x * bytes_per_pixel;

                    memmove(dest_row, source_row,
                        image_blit->width * bytes_per_pixel);
                }
            }
            else {
                assert(false);
            }

            break;
        default:
            log_warning("EmuNV2A: Unknown NV_IMAGE_BLIT Method: 0x%08X\n", method);
        }
        break;
    }

    case NV_KELVIN_PRIMITIVE:
    {
        switch (method) {
        case NV097_SET_CONTEXT_DMA_NOTIFIES:
            kelvin->dma_notifies = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_A:
            m_PGRAPH.dma_a = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_B:
            m_PGRAPH.dma_b = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_STATE:
            kelvin->dma_state = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_COLOR:
            log_debug("TODO: pgraph_update_surface\n");
            /* try to get any straggling draws in before the surface's changed :/ */
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.dma_color = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_ZETA:
            m_PGRAPH.dma_zeta = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_VERTEX_A:
            m_PGRAPH.dma_vertex_a = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_VERTEX_B:
            m_PGRAPH.dma_vertex_b = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_SEMAPHORE:
            kelvin->dma_semaphore = parameter;
            break;
        case NV097_SET_CONTEXT_DMA_REPORT:
            m_PGRAPH.dma_report = parameter;
            break;
        case NV097_SET_SURFACE_CLIP_HORIZONTAL:
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.surface_shape.clip_x =
                GET_MASK(parameter, NV097_SET_SURFACE_CLIP_HORIZONTAL_X);
            m_PGRAPH.surface_shape.clip_width =
                GET_MASK(parameter, NV097_SET_SURFACE_CLIP_HORIZONTAL_WIDTH);
            break;
        case NV097_SET_SURFACE_CLIP_VERTICAL:
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.surface_shape.clip_y =
                GET_MASK(parameter, NV097_SET_SURFACE_CLIP_VERTICAL_Y);
            m_PGRAPH.surface_shape.clip_height =
                GET_MASK(parameter, NV097_SET_SURFACE_CLIP_VERTICAL_HEIGHT);
            break;
        case NV097_SET_SURFACE_FORMAT:
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.surface_shape.color_format =
                GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_COLOR);
            m_PGRAPH.surface_shape.zeta_format =
                GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_ZETA);
            m_PGRAPH.surface_type =
                GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_TYPE);
            m_PGRAPH.surface_shape.anti_aliasing =
                GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_ANTI_ALIASING);
            m_PGRAPH.surface_shape.log_width =
                GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_WIDTH);
            m_PGRAPH.surface_shape.log_height =
                GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_HEIGHT);
            break;
        case NV097_SET_SURFACE_PITCH:
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.surface_color.pitch =
                GET_MASK(parameter, NV097_SET_SURFACE_PITCH_COLOR);
            m_PGRAPH.surface_zeta.pitch =
                GET_MASK(parameter, NV097_SET_SURFACE_PITCH_ZETA);
            break;
        case NV097_SET_SURFACE_COLOR_OFFSET:
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.surface_color.offset = parameter;
            break;
        case NV097_SET_SURFACE_ZETA_OFFSET:
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            m_PGRAPH.surface_zeta.offset = parameter;
            break;
        case NV097_SET_COMBINER_SPECULAR_FOG_CW0:
            m_PGRAPH.regs[NV_PGRAPH_COMBINESPECFOG0] = parameter;
            break;
        case NV097_SET_COMBINER_SPECULAR_FOG_CW1:
            m_PGRAPH.regs[NV_PGRAPH_COMBINESPECFOG1] = parameter;
            break;
            CASE_4(NV097_SET_TEXTURE_ADDRESS, 64) :
                slot = (method - NV097_SET_TEXTURE_ADDRESS) / 64;
            m_PGRAPH.regs[NV_PGRAPH_TEXADDRESS0 + slot * 4] = parameter;
            break;
        case NV097_SET_CONTROL0:
        {
            log_debug("TODO: pgraph_update_surface\n");
            //pgraph_update_surface(d, false, true, true);

            bool stencil_write_enable =
                parameter & NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE;
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE,
                stencil_write_enable);

            uint32_t z_format = GET_MASK(parameter, NV097_SET_CONTROL0_Z_FORMAT);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_Z_FORMAT, z_format);

            bool z_perspective =
                parameter & NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE;
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_Z_PERSPECTIVE_ENABLE,
                z_perspective);
            break;
        }

        case NV097_SET_FOG_MODE:
        {
            /* FIXME: There is also NV_PGRAPH_CSV0_D_FOG_MODE */
            unsigned int mode;
            switch (parameter) {
            case NV097_SET_FOG_MODE_V_LINEAR:
                mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR; break;
            case NV097_SET_FOG_MODE_V_EXP:
                mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP; break;
            case NV097_SET_FOG_MODE_V_EXP2:
                mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2; break;
            case NV097_SET_FOG_MODE_V_EXP_ABS:
                mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP_ABS; break;
            case NV097_SET_FOG_MODE_V_EXP2_ABS:
                mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2_ABS; break;
            case NV097_SET_FOG_MODE_V_LINEAR_ABS:
                mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR_ABS; break;
            default:
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_3], NV_PGRAPH_CONTROL_3_FOG_MODE,
                mode);
            break;
        }
        case NV097_SET_FOG_GEN_MODE:
        {
            unsigned int mode;
            switch (parameter) {
            case NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA:
                mode = NV_PGRAPH_CSV0_D_FOGGENMODE_SPEC_ALPHA; break;
            case NV097_SET_FOG_GEN_MODE_V_RADIAL:
                mode = NV_PGRAPH_CSV0_D_FOGGENMODE_RADIAL; break;
            case NV097_SET_FOG_GEN_MODE_V_PLANAR:
                mode = NV_PGRAPH_CSV0_D_FOGGENMODE_PLANAR; break;
            case NV097_SET_FOG_GEN_MODE_V_ABS_PLANAR:
                mode = NV_PGRAPH_CSV0_D_FOGGENMODE_ABS_PLANAR; break;
            case NV097_SET_FOG_GEN_MODE_V_FOG_X:
                mode = NV_PGRAPH_CSV0_D_FOGGENMODE_FOG_X; break;
            default:
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_FOGGENMODE, mode);
            break;
        }
        case NV097_SET_FOG_ENABLE:
            /*
            FIXME: There is also:
            SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_FOGENABLE,
            parameter);
            */
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_3], NV_PGRAPH_CONTROL_3_FOGENABLE,
                parameter);
            break;
        case NV097_SET_FOG_COLOR:
        {
            /* PGRAPH channels are ARGB, parameter channels are ABGR */
            uint8_t red = GET_MASK(parameter, NV097_SET_FOG_COLOR_RED);
            uint8_t green = GET_MASK(parameter, NV097_SET_FOG_COLOR_GREEN);
            uint8_t blue = GET_MASK(parameter, NV097_SET_FOG_COLOR_BLUE);
            uint8_t alpha = GET_MASK(parameter, NV097_SET_FOG_COLOR_ALPHA);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_RED, red);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_GREEN, green);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_BLUE, blue);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_ALPHA, alpha);
            break;
        }
        case NV097_SET_ALPHA_TEST_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_ALPHATESTENABLE, parameter);
            break;
        case NV097_SET_BLEND_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_EN, parameter);
            break;
        case NV097_SET_CULL_FACE_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_CULLENABLE,
                parameter);
            break;
        case NV097_SET_DEPTH_TEST_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZENABLE,
                parameter);
            break;
        case NV097_SET_DITHER_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_DITHERENABLE, parameter);
            break;
        case NV097_SET_LIGHTING_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CSV0_C], NV_PGRAPH_CSV0_C_LIGHTING,
                parameter);
            break;
        case NV097_SET_SKIN_MODE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_SKIN,
                parameter);
            break;
        case NV097_SET_STENCIL_TEST_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_1],
                NV_PGRAPH_CONTROL_1_STENCIL_TEST_ENABLE, parameter);
            break;
        case NV097_SET_POLY_OFFSET_POINT_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_POFFSETPOINTENABLE, parameter);
            break;
        case NV097_SET_POLY_OFFSET_LINE_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_POFFSETLINEENABLE, parameter);
            break;
        case NV097_SET_POLY_OFFSET_FILL_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_POFFSETFILLENABLE, parameter);
            break;
        case NV097_SET_ALPHA_FUNC:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_ALPHAFUNC, parameter & 0xF);
            break;
        case NV097_SET_ALPHA_REF:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_ALPHAREF, parameter);
            break;
        case NV097_SET_BLEND_FUNC_SFACTOR:
        {
            unsigned int factor;
            switch (parameter) {
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ZERO:
                factor = NV_PGRAPH_BLEND_SFACTOR_ZERO; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR:
                factor = NV_PGRAPH_BLEND_SFACTOR_SRC_COLOR; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_COLOR; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA:
                factor = NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_ALPHA; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA:
                factor = NV_PGRAPH_BLEND_SFACTOR_DST_ALPHA; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_ALPHA; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR:
                factor = NV_PGRAPH_BLEND_SFACTOR_DST_COLOR; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_COLOR; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE:
                factor = NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA_SATURATE; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_COLOR:
                factor = NV_PGRAPH_BLEND_SFACTOR_CONSTANT_COLOR; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_COLOR:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_COLOR; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_ALPHA:
                factor = NV_PGRAPH_BLEND_SFACTOR_CONSTANT_ALPHA; break;
            case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_ALPHA:
                factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
            default:
                log_warning("Unknown blend source factor: 0x%x\n", parameter);
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_SFACTOR, factor);

            break;
        }

        case NV097_SET_BLEND_FUNC_DFACTOR:
        {
            unsigned int factor;
            switch (parameter) {
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ZERO:
                factor = NV_PGRAPH_BLEND_DFACTOR_ZERO; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_COLOR:
                factor = NV_PGRAPH_BLEND_DFACTOR_SRC_COLOR; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_COLOR:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_COLOR; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA:
                factor = NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_ALPHA; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_DST_ALPHA:
                factor = NV_PGRAPH_BLEND_DFACTOR_DST_ALPHA; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_ALPHA:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_ALPHA; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_DST_COLOR:
                factor = NV_PGRAPH_BLEND_DFACTOR_DST_COLOR; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_COLOR:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_COLOR; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA_SATURATE:
                factor = NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA_SATURATE; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_COLOR:
                factor = NV_PGRAPH_BLEND_DFACTOR_CONSTANT_COLOR; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_COLOR:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_COLOR; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_ALPHA:
                factor = NV_PGRAPH_BLEND_DFACTOR_CONSTANT_ALPHA; break;
            case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_ALPHA:
                factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
            default:
                log_warning("Unknown blend destination factor: 0x%x\n", parameter);
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_DFACTOR, factor);

            break;
        }

        case NV097_SET_BLEND_COLOR:
            m_PGRAPH.regs[NV_PGRAPH_BLENDCOLOR] = parameter;
            break;

        case NV097_SET_BLEND_EQUATION:
        {
            unsigned int equation;
            switch (parameter) {
            case NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT:
                equation = 0; break;
            case NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT:
                equation = 1; break;
            case NV097_SET_BLEND_EQUATION_V_FUNC_ADD:
                equation = 2; break;
            case NV097_SET_BLEND_EQUATION_V_MIN:
                equation = 3; break;
            case NV097_SET_BLEND_EQUATION_V_MAX:
                equation = 4; break;
            case NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT_SIGNED:
                equation = 5; break;
            case NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED:
                equation = 6; break;
            default:
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_EQN, equation);

            break;
        }

        case NV097_SET_DEPTH_FUNC:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZFUNC,
                parameter & 0xF);
            break;

        case NV097_SET_COLOR_MASK:
        {
            m_PGRAPH.surface_color.write_enabled_cache |= pgraph_color_write_enabled();

            bool alpha = parameter & NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE;
            bool red = parameter & NV097_SET_COLOR_MASK_RED_WRITE_ENABLE;
            bool green = parameter & NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE;
            bool blue = parameter & NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE;
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE, alpha);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE, red);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE, green);
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE, blue);
            break;
        }
        case NV097_SET_DEPTH_MASK:
            m_PGRAPH.surface_zeta.write_enabled_cache |= pgraph_zeta_write_enabled();

            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_0],
                NV_PGRAPH_CONTROL_0_ZWRITEENABLE, parameter);
            break;
        case NV097_SET_STENCIL_MASK:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_1],
                NV_PGRAPH_CONTROL_1_STENCIL_MASK_WRITE, parameter);
            break;
        case NV097_SET_STENCIL_FUNC:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_1],
                NV_PGRAPH_CONTROL_1_STENCIL_FUNC, parameter & 0xF);
            break;
        case NV097_SET_STENCIL_FUNC_REF:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_1],
                NV_PGRAPH_CONTROL_1_STENCIL_REF, parameter);
            break;
        case NV097_SET_STENCIL_FUNC_MASK:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_1],
                NV_PGRAPH_CONTROL_1_STENCIL_MASK_READ, parameter);
            break;
        case NV097_SET_STENCIL_OP_FAIL:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_2],
                NV_PGRAPH_CONTROL_2_STENCIL_OP_FAIL,
                kelvin_map_stencil_op(parameter));
            break;
        case NV097_SET_STENCIL_OP_ZFAIL:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_2],
                NV_PGRAPH_CONTROL_2_STENCIL_OP_ZFAIL,
                kelvin_map_stencil_op(parameter));
            break;
        case NV097_SET_STENCIL_OP_ZPASS:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CONTROL_2],
                NV_PGRAPH_CONTROL_2_STENCIL_OP_ZPASS,
                kelvin_map_stencil_op(parameter));
            break;

        case NV097_SET_POLYGON_OFFSET_SCALE_FACTOR:
            m_PGRAPH.regs[NV_PGRAPH_ZOFFSETFACTOR] = parameter;
            break;
        case NV097_SET_POLYGON_OFFSET_BIAS:
            m_PGRAPH.regs[NV_PGRAPH_ZOFFSETBIAS] = parameter;
            break;
        case NV097_SET_FRONT_POLYGON_MODE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_FRONTFACEMODE,
                kelvin_map_polygon_mode(parameter));
            break;
        case NV097_SET_BACK_POLYGON_MODE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_BACKFACEMODE,
                kelvin_map_polygon_mode(parameter));
            break;
        case NV097_SET_CLIP_MIN:
            m_PGRAPH.regs[NV_PGRAPH_ZCLIPMIN] = parameter;
            break;
        case NV097_SET_CLIP_MAX:
            m_PGRAPH.regs[NV_PGRAPH_ZCLIPMAX] = parameter;
            break;
        case NV097_SET_CULL_FACE:
        {
            unsigned int face;
            switch (parameter) {
            case NV097_SET_CULL_FACE_V_FRONT:
                face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT; break;
            case NV097_SET_CULL_FACE_V_BACK:
                face = NV_PGRAPH_SETUPRASTER_CULLCTRL_BACK; break;
            case NV097_SET_CULL_FACE_V_FRONT_AND_BACK:
                face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT_AND_BACK; break;
            default:
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_CULLCTRL,
                face);
            break;
        }
        case NV097_SET_FRONT_FACE:
        {
            bool ccw;
            switch (parameter) {
            case NV097_SET_FRONT_FACE_V_CW:
                ccw = false; break;
            case NV097_SET_FRONT_FACE_V_CCW:
                ccw = true; break;
            default:
                log_warning("Unknown front face: 0x%x\n", parameter);
                assert(false);
                break;
            }
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_SETUPRASTER],
                NV_PGRAPH_SETUPRASTER_FRONTFACE,
                ccw ? 1 : 0);
            break;
        }
        case NV097_SET_NORMALIZATION_ENABLE:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CSV0_C],
                NV_PGRAPH_CSV0_C_NORMALIZATION_ENABLE,
                parameter);
            break;

        case NV097_SET_LIGHT_ENABLE_MASK:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CSV0_D],
                NV_PGRAPH_CSV0_D_LIGHTS,
                parameter);
            break;

            CASE_4(NV097_SET_TEXGEN_S, 16) : {
                slot = (method - NV097_SET_TEXGEN_S) / 16;
                unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
                    : NV_PGRAPH_CSV1_B;
                unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_S
                    : NV_PGRAPH_CSV1_A_T0_S;
                SET_MASK(m_PGRAPH.regs[reg], mask, kelvin_map_texgen(parameter, 0));
                break;
            }
            CASE_4(NV097_SET_TEXGEN_T, 16) : {
                slot = (method - NV097_SET_TEXGEN_T) / 16;
                unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
                    : NV_PGRAPH_CSV1_B;
                unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_T
                    : NV_PGRAPH_CSV1_A_T0_T;
                SET_MASK(m_PGRAPH.regs[reg], mask, kelvin_map_texgen(parameter, 1));
                break;
            }
            CASE_4(NV097_SET_TEXGEN_R, 16) : {
                slot = (method - NV097_SET_TEXGEN_R) / 16;
                unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
                    : NV_PGRAPH_CSV1_B;
                unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_R
                    : NV_PGRAPH_CSV1_A_T0_R;
                SET_MASK(m_PGRAPH.regs[reg], mask, kelvin_map_texgen(parameter, 2));
                break;
            }
            CASE_4(NV097_SET_TEXGEN_Q, 16) : {
                slot = (method - NV097_SET_TEXGEN_Q) / 16;
                unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
                    : NV_PGRAPH_CSV1_B;
                unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_Q
                    : NV_PGRAPH_CSV1_A_T0_Q;
                SET_MASK(m_PGRAPH.regs[reg], mask, kelvin_map_texgen(parameter, 3));
                break;
            }
            CASE_4(NV097_SET_TEXTURE_MATRIX_ENABLE, 4) :
                slot = (method - NV097_SET_TEXTURE_MATRIX_ENABLE) / 4;
            m_PGRAPH.texture_matrix_enable[slot] = parameter;
            break;

        case NV097_SET_TEXGEN_VIEW_MODEL:
            SET_MASK(m_PGRAPH.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_TEXGEN_REF, parameter);
            break;
        default:
            if (method >= NV097_SET_COMBINER_ALPHA_ICW && method <= NV097_SET_COMBINER_ALPHA_ICW + 28) {
                slot = (method - NV097_SET_COMBINER_ALPHA_ICW) / 4;
                m_PGRAPH.regs[NV_PGRAPH_COMBINEALPHAI0 + slot * 4] = parameter;
                break;
            }

            if (method >= NV097_SET_PROJECTION_MATRIX && method <= NV097_SET_PROJECTION_MATRIX + 0x3c) {
                slot = (method - NV097_SET_PROJECTION_MATRIX) / 4;
                // pg->projection_matrix[slot] = *(float*)&parameter;
                unsigned int row = NV_IGRAPH_XF_XFCTX_PMAT0 + slot / 4;
                m_PGRAPH.vsh_constants[row][slot % 4] = parameter;
                m_PGRAPH.vsh_constants_dirty[row] = true;
                break;
            }

            if (method >= NV097_SET_MODEL_VIEW_MATRIX && method <= NV097_SET_MODEL_VIEW_MATRIX + 0xfc) {
                slot = (method - NV097_SET_MODEL_VIEW_MATRIX) / 4;
                unsigned int matnum = slot / 16;
                unsigned int entry = slot % 16;
                unsigned int row = NV_IGRAPH_XF_XFCTX_MMAT0 + matnum * 8 + entry / 4;
                m_PGRAPH.vsh_constants[row][entry % 4] = parameter;
                m_PGRAPH.vsh_constants_dirty[row] = true;
                break;
            }

            if (method >= NV097_SET_INVERSE_MODEL_VIEW_MATRIX && method <= NV097_SET_INVERSE_MODEL_VIEW_MATRIX + 0xfc) {
                slot = (method - NV097_SET_INVERSE_MODEL_VIEW_MATRIX) / 4;
                unsigned int matnum = slot / 16;
                unsigned int entry = slot % 16;
                unsigned int row = NV_IGRAPH_XF_XFCTX_IMMAT0 + matnum * 8 + entry / 4;
                m_PGRAPH.vsh_constants[row][entry % 4] = parameter;
                m_PGRAPH.vsh_constants_dirty[row] = true;
                break;
            }

            if (method >= NV097_SET_COMPOSITE_MATRIX && method <= NV097_SET_COMPOSITE_MATRIX + 0x3c) {
                slot = (method - NV097_SET_COMPOSITE_MATRIX) / 4;
                unsigned int row = NV_IGRAPH_XF_XFCTX_CMAT0 + slot / 4;
                m_PGRAPH.vsh_constants[row][slot % 4] = parameter;
                m_PGRAPH.vsh_constants_dirty[row] = true;
                break;
            }

            if (method >= NV097_SET_TEXTURE_MATRIX && method <= NV097_SET_TEXTURE_MATRIX + 0xfc) {
                slot = (method - NV097_SET_TEXTURE_MATRIX) / 4;
                unsigned int tex = slot / 16;
                unsigned int entry = slot % 16;
                unsigned int row = NV_IGRAPH_XF_XFCTX_T0MAT + tex * 8 + entry / 4;
                m_PGRAPH.vsh_constants[row][entry % 4] = parameter;
                m_PGRAPH.vsh_constants_dirty[row] = true;
                break;
            }

            if (method >= NV097_SET_FOG_PARAMS && method <= NV097_SET_FOG_PARAMS + 8) {
                slot = (method - NV097_SET_FOG_PARAMS) / 4;
                if (slot < 2) {
                    m_PGRAPH.regs[NV_PGRAPH_FOGPARAM0 + slot * 4] = parameter;
                }
                else {
                    /* FIXME: No idea where slot = 2 is */
                }

                m_PGRAPH.ltctxa[NV_IGRAPH_XF_LTCTXA_FOG_K][slot] = parameter;
                m_PGRAPH.ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FOG_K] = true;
                break;
            }

            /* Handles NV097_SET_TEXGEN_PLANE_S,T,R,Q */
            if (method >= NV097_SET_TEXGEN_PLANE_S && method <= NV097_SET_TEXGEN_PLANE_S + 0xfc) {
                slot = (method - NV097_SET_TEXGEN_PLANE_S) / 4;
                unsigned int tex = slot / 16;
                unsigned int entry = slot % 16;
                unsigned int row = NV_IGRAPH_XF_XFCTX_TG0MAT + tex * 8 + entry / 4;
                m_PGRAPH.vsh_constants[row][entry % 4] = parameter;
                m_PGRAPH.vsh_constants_dirty[row] = true;
                break;
            }

            if (method >= NV097_SET_FOG_PLANE && method <= NV097_SET_FOG_PLANE + 12) {
                slot = (method - NV097_SET_FOG_PLANE) / 4;
                m_PGRAPH.vsh_constants[NV_IGRAPH_XF_XFCTX_FOG][slot] = parameter;
                m_PGRAPH.vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_FOG] = true;
                break;
            }

            if (method >= NV097_SET_SCENE_AMBIENT_COLOR && method <= NV097_SET_SCENE_AMBIENT_COLOR + 8) {
                slot = (method - NV097_SET_SCENE_AMBIENT_COLOR) / 4;
                // ??
                m_PGRAPH.ltctxa[NV_IGRAPH_XF_LTCTXA_FR_AMB][slot] = parameter;
                m_PGRAPH.ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FR_AMB] = true;
                break;
            }

            if (method >= NV097_SET_VIEWPORT_OFFSET && method <= NV097_SET_VIEWPORT_OFFSET + 12) {
                slot = (method - NV097_SET_VIEWPORT_OFFSET) / 4;
                m_PGRAPH.vsh_constants[NV_IGRAPH_XF_XFCTX_VPOFF][slot] = parameter;
                m_PGRAPH.vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPOFF] = true;
                break;
            }

            if (method >= NV097_SET_EYE_POSITION && method <= NV097_SET_EYE_POSITION + 12) {
                slot = (method - NV097_SET_EYE_POSITION) / 4;
                m_PGRAPH.vsh_constants[NV_IGRAPH_XF_XFCTX_EYEP][slot] = parameter;
                m_PGRAPH.vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_EYEP] = true;
                break;
            }

            if (method >= NV097_SET_COMBINER_FACTOR0 && method <= NV097_SET_COMBINER_FACTOR0 + 28) {
                slot = (method - NV097_SET_COMBINER_FACTOR0) / 4;
                m_PGRAPH.regs[NV_PGRAPH_COMBINEFACTOR0 + slot * 4] = parameter;
                break;
            }

            if (method >= NV097_SET_COMBINER_FACTOR1 && method <= NV097_SET_COMBINER_FACTOR1 + 28) {
                slot = (method - NV097_SET_COMBINER_FACTOR1) / 4;
                m_PGRAPH.regs[NV_PGRAPH_COMBINEFACTOR1 + slot * 4] = parameter;
                break;
            }

            if (method >= NV097_SET_COMBINER_ALPHA_OCW && method <= NV097_SET_COMBINER_ALPHA_OCW + 28) {
                slot = (method - NV097_SET_COMBINER_ALPHA_OCW) / 4;
                m_PGRAPH.regs[NV_PGRAPH_COMBINEALPHAO0 + slot * 4] = parameter;
                break;
            }

            if (method >= NV097_SET_COMBINER_COLOR_ICW && method <= NV097_SET_COMBINER_COLOR_ICW + 28) {
                slot = (method - NV097_SET_COMBINER_COLOR_ICW) / 4;
                m_PGRAPH.regs[NV_PGRAPH_COMBINECOLORI0 + slot * 4] = parameter;
                break;
            }

            if (method >= NV097_SET_VIEWPORT_SCALE && method <= NV097_SET_VIEWPORT_SCALE + 12) {
                slot = (method - NV097_SET_VIEWPORT_SCALE) / 4;
                m_PGRAPH.vsh_constants[NV_IGRAPH_XF_XFCTX_VPSCL][slot] = parameter;
                m_PGRAPH.vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPSCL] = true;
                break;
            }

            log_warning("EmuNV2A: Unknown NV_KELVIN_PRIMITIVE Method: 0x%08X\n", method);
        }
        break;
    }

    default:
        log_warning("EmuNV2A: Unknown Graphics Class/Method 0x%08X/0x%08X\n", object->graphics_class, method);
        break;
    }
}

void NV2ADevice::pfifo_run_pusher() {
    uint8_t channel_id;
    ChannelControl *control;
    Cache1State *state;
    CacheEntry *command;
    uint8_t *dma;
    uint32_t dma_len;
    uint32_t word;

    /* TODO: How is cache1 selected? */
    state = &m_PFIFO.cache1;
    channel_id = state->channel_id;
    control = &m_User.channel_control[channel_id];

    if (!state->push_enabled) return;

    /* only handling DMA for now... */

    /* Channel running DMA */
    uint32_t channel_modes = m_PFIFO.regs[NV_PFIFO_MODE];
    assert(channel_modes & (1 << channel_id));
    assert(state->mode == FIFO_DMA);

    if (!state->dma_push_enabled) return;
    if (state->dma_push_suspended) return;

    /* We're running so there should be no pending errors... */
    assert(state->error == NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE);

    dma = (uint8_t*)nv_dma_map(state->dma_instance, &dma_len);

    log_debug("DMA pusher: max 0x%08X, 0x%08X - 0x%08X\n",
        dma_len, control->dma_get, control->dma_put);

    /* based on the convenient pseudocode in envytools */
    /* See: http://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html */
    while (control->dma_get != control->dma_put) {
        if (control->dma_get >= dma_len) {
            state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
            break;
        }

        word = ldl_le_p((uint32_t*)(dma + control->dma_get));
        control->dma_get += 4;

        if (state->method_count) {
            /* data word of methods command */
            state->data_shadow = word;

            CacheEntry* command = (CacheEntry*)calloc(1, sizeof(CacheEntry));
            command->method = state->method;
            command->subchannel = state->subchannel;
            command->nonincreasing = state->method_nonincreasing;
            command->parameter = word;

            std::lock_guard<std::mutex> lk(state->mutex);
            state->cache.push(command);
            state->cache_cond.notify_all();

            if (!state->method_nonincreasing) {
                state->method += 4;
            }

            state->method_count--;
            state->dcount++;
        }
        else {
            /* no command active - this is the first word of a new one */
            state->rsvd_shadow = word;
            /* match all forms */
            if ((word & 0xe0000003) == 0x20000000) {
                /* old jump */
                state->get_jmp_shadow = control->dma_get;
                control->dma_get = word & 0x1fffffff;
                log_debug("pb OLD_JMP 0x%08X\n", control->dma_get);
            }
            else if ((word & 3) == 1) {
                /* jump */
                state->get_jmp_shadow = control->dma_get;
                control->dma_get = word & 0xfffffffc;
                log_debug("pb JMP 0x%08X\n", control->dma_get);
            }
            else if ((word & 3) == 2) {
                /* call */
                if (state->subroutine_active) {
                    state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
                    break;
                }
                state->subroutine_return = control->dma_get;
                state->subroutine_active = true;
                control->dma_get = word & 0xfffffffc;
                log_debug("pb CALL 0x%08X\n", control->dma_get);
            }
            else if (word == 0x00020000) {
                /* return */
                if (!state->subroutine_active) {
                    state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
                    break;
                }
                control->dma_get = state->subroutine_return;
                state->subroutine_active = false;
                log_debug("pb RET 0x%08X\n", control->dma_get);
            }
            else if ((word & 0xe0030003) == 0) {
                /* increasing methods */
                state->method = word & 0x1fff;
                state->subchannel = (word >> 13) & 7;
                state->method_count = (word >> 18) & 0x7ff;
                state->method_nonincreasing = false;
                state->dcount = 0;
            }
            else if ((word & 0xe0030003) == 0x40000000) {
                /* non-increasing methods */
                state->method = word & 0x1fff;
                state->subchannel = (word >> 13) & 7;
                state->method_count = (word >> 18) & 0x7ff;
                state->method_nonincreasing = true;
                state->dcount = 0;
            }
            else {
                log_debug("pb reserved cmd 0x%08X - 0x%08X\n",
                    control->dma_get, word);
                state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
                break;
            }
        }
    }

    log_debug("DMA pusher done: max 0x%08X, 0x%08X - 0x%08X\n",
        dma_len, control->dma_get, control->dma_put);

    if (state->error) {
        log_warning("pb error: %d\n", state->error);
        assert(false);

        state->dma_push_suspended = true;

        m_PFIFO.pending_interrupts |= NV_PFIFO_INTR_0_DMA_PUSHER;
        UpdateIRQ();
    }
}

void NV2ADevice::PFIFO_Puller_Thread(NV2ADevice *nv2a) {
    Thread_SetName("[HW] NV2A PFIFO Puller");

    Cache1State *state = &nv2a->m_PFIFO.cache1;
    while (nv2a->m_running) {
        // Scope the lock so that it automatically unlocks at tne end of this block
        {
            std::unique_lock<std::mutex> lk(state->mutex);

            while (state->cache.empty() || !state->pull_enabled) {
                state->cache_cond.wait(lk);
                if (!nv2a->m_running) {
                    break;
                }
            }

            // Copy cache to working_cache
            while (!state->cache.empty()) {
                state->working_cache.push(state->cache.front());
                state->cache.pop();
            }
        }

        while (!state->working_cache.empty()) {
            CacheEntry* command = state->working_cache.front();
            state->working_cache.pop();

            if (command->method == 0) {
                // qemu_mutex_lock_iothread();
                RAMHTEntry entry = nv2a->ramht_lookup(command->parameter);
                assert(entry.valid);

                assert(entry.channel_id == state->channel_id);
                // qemu_mutex_unlock_iothread();

                switch (entry.engine) {
                case ENGINE_GRAPHICS:
                    nv2a->pgraph_context_switch(entry.channel_id);
                    nv2a->pgraph_wait_fifo_access();
                    nv2a->pgraph_method(command->subchannel, 0, entry.instance);
                    break;
                default:
                    assert(false);
                    break;
                }

                /* the engine is bound to the subchannel */
                std::lock_guard<std::mutex> lk(nv2a->m_PFIFO.cache1.mutex);
                state->bound_engines[command->subchannel] = entry.engine;
                state->last_engine = entry.engine;
            }
            else if (command->method >= 0x100) {
                /* method passed to engine */

                uint32_t parameter = command->parameter;

                /* methods that take objects.
                * TODO: Check this range is correct for the nv2a */
                if (command->method >= 0x180 && command->method < 0x200) {
                    //qemu_mutex_lock_iothread();
                    RAMHTEntry entry = nv2a->ramht_lookup(parameter);
                    assert(entry.valid);
                    assert(entry.channel_id == state->channel_id);
                    parameter = entry.instance;
                    //qemu_mutex_unlock_iothread();
                }

                // qemu_mutex_lock(&state->cache_lock);
                enum FIFOEngine engine = state->bound_engines[command->subchannel];
                // qemu_mutex_unlock(&state->cache_lock);

                switch (engine) {
                case ENGINE_GRAPHICS:
                    nv2a->pgraph_wait_fifo_access();
                    nv2a->pgraph_method(command->subchannel, command->method, parameter);
                    break;
                default:
                    assert(false);
                    break;
                }

                // qemu_mutex_lock(&state->cache_lock);
                state->last_engine = state->bound_engines[command->subchannel];
                // qemu_mutex_unlock(&state->cache_lock);
            }

            free(command);
        }
    }
}

void NV2ADevice::UpdateIRQ() {
    if (m_PFIFO.pending_interrupts & m_PFIFO.enabled_interrupts) {
        m_PMC.pendingInterrupts |= NV_PMC_INTR_0_PFIFO;
    }
    else {
        m_PMC.pendingInterrupts &= ~NV_PMC_INTR_0_PFIFO;
    }

    if (m_PCRTC.pendingInterrupts & m_PCRTC.enabledInterrupts) {
        m_PMC.pendingInterrupts |= NV_PMC_INTR_0_PCRTC;
    }
    else {
        m_PMC.pendingInterrupts &= ~NV_PMC_INTR_0_PCRTC;
    }

    if (m_PGRAPH.pending_interrupts & m_PGRAPH.enabled_interrupts) {
        m_PMC.pendingInterrupts |= NV_PMC_INTR_0_PGRAPH;
    }
    else {
        m_PMC.pendingInterrupts &= ~NV_PMC_INTR_0_PGRAPH;
    }

    uint8_t irq = Read8(m_configSpace, PCI_INTERRUPT_PIN);
    if (m_PMC.pendingInterrupts && m_PMC.enabledInterrupts) {
        m_pic->RaiseIRQ(irq);
    }
    else {
        m_pic->LowerIRQ(irq);
    }
}

void NV2ADevice::VBlankThread(NV2ADevice *nv2a) {
    Thread_SetName("[HW] NV2A VBlank");

    using namespace std::chrono;
    auto nextStop = high_resolution_clock::now();
    auto interval = duration<long long, std::ratio<1, 1000000>>((long long)(1000000.0f / 60.0f));

    while (nv2a->m_running) {
        // TODO: wait for a condition variable instead of checking like this
        if (nv2a->m_PCRTC.enabledInterrupts & NV_PCRTC_INTR_0_VBLANK) {
            nv2a->m_PCRTC.pendingInterrupts |= NV_PCRTC_INTR_0_VBLANK;
            nv2a->UpdateIRQ();
        }

        nextStop += interval;
        std::this_thread::sleep_until(nextStop);
    }
}

}
