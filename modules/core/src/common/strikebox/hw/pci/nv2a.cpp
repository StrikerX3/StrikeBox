#include "strikebox/hw/pci/nv2a.h"

#include "strikebox/log.h"

namespace strikebox {

NV2ADevice::NV2ADevice(uint8_t *pSystemRAM, uint32_t systemRAMSize, IRQHandler& irqHandler)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A0, 0xA1,
        0x03, 0x00, 0x00) // VGA-compatible controller
    , m_pSystemRAM(pSystemRAM)
    , m_systemRAMSize(systemRAMSize)
    , m_irqHandler(irqHandler)
{
    RegisterEngine(m_pmc);
    RegisterEngine(m_pbus);
    RegisterEngine(m_pfifo);
    RegisterEngine(m_prma);
    RegisterEngine(m_pvideo);
    RegisterEngine(m_ptimer);
    RegisterEngine(m_pcounter);
    RegisterEngine(m_pnvio);
    RegisterEngine(m_pfb);
    RegisterEngine(m_pstraps);
    RegisterEngine(m_prom);
    RegisterEngine(m_pgraph);
    RegisterEngine(m_pcrtc);
    RegisterEngine(m_prmcio);
    RegisterEngine(m_pramdac);
    RegisterEngine(m_prmdio);
    RegisterEngine(m_pramin);
    RegisterEngine(m_user);
}

NV2ADevice::~NV2ADevice() {
}

// PCI Device functions

void NV2ADevice::Init() {
    RegisterBAR(0, 0x1000000, PCI_BAR_TYPE_MEMORY); // 0xFD000000 - 0xFDFFFFFF
    RegisterBAR(1, 0x8000000, PCI_BAR_TYPE_MEMORY | PCI_BAR_MEMORY_PREFETCHABLE); // 0xF0000000 - 0xF7FFFFFF
    RegisterBAR(2, 0x80000, PCI_BAR_TYPE_MEMORY | PCI_BAR_MEMORY_PREFETCHABLE); // 0x0 - 0x7FFFF

    Write8(m_configSpace, PCI_LATENCY_TIMER, 0xf8);

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST | PCI_STATUS_DEVSEL_MEDIUM);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x60);
    Write8(m_configSpace, PCI_MIN_GNT, 0x05);
    Write8(m_configSpace, PCI_MAX_LAT, 0x01);

    // Capability list
    Write8(m_configSpace, 0x60, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x61, 0x44);

    Write8(m_configSpace, 0x44, PCI_CAP_ID_AGP);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x20);
    Write32(m_configSpace, 0x48, 0x1f000017);
    Write16(m_configSpace, 0x4c, 0x1f00);
    Write32(m_configSpace, 0x54, 0x1);
    Write32(m_configSpace, 0x58, 0x23d6ce);
    Write32(m_configSpace, 0x5c, 0xf);
    Write32(m_configSpace, 0x60, 0x24401);
    Write32(m_configSpace, 0x80, 0x2b16d065);

    Reset();
}

void NV2ADevice::Reset() {
}

void NV2ADevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_warning("NV2ADevice::PCIIORead:  Unexpected I/O read!   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    *value = 0;
}

void NV2ADevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::PCIIOWrite: Unexpected I/O write!  bar = %d,  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);
}

void NV2ADevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    if (barIndex == 0) {
        auto opt_eng = FindEngine(addr);
        if (opt_eng) {
            auto& eng = opt_eng->get();
            *value = eng.Read(addr - eng.GetOffset(), size);
        }
        else {
            log_spew("NV2ADevice::PCIMMIORead:   Unmapped read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
        }
    }
    else {
        log_spew("NV2ADevice::PCIMMIORead:   Unimplemented read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
        *value = 0;
    }
}

void NV2ADevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    if (barIndex == 0) {
        auto opt_eng = FindEngine(addr);
        if (opt_eng) {
            auto& eng = opt_eng->get();
            eng.Write(addr - eng.GetOffset(), value, size);
        }
        else {
            log_spew("NV2ADevice::PCIMMIOWrite:  Unmapped write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
        }
    }
    else {
        log_spew("NV2ADevice::PCIMMIOWrite:  Unimplemented write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
    }
}

void NV2ADevice::RegisterEngine(nv2a::NV2AEngine& engine) {
    engines.insert({ engine.GetOffset() + engine.GetLength() - 1, engine });
}

std::optional<std::reference_wrapper<nv2a::NV2AEngine>> NV2ADevice::FindEngine(const uint32_t address) {
    auto entry = engines.lower_bound(address);
    if (entry != engines.end()) {
        auto& engine = entry->second;
        if (engine.Contains(address)) {
            return engine;
        }
    }
    return std::nullopt;
}

}
