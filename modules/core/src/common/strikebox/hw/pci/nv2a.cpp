#include "strikebox/hw/pci/nv2a.h"

#include "strikebox/log.h"

namespace strikebox {

NV2ADevice::NV2ADevice(uint8_t *pSystemRAM, uint32_t systemRAMSize, IRQHandler& irqHandler)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A0, 0xA2,
        0x03, 0x00, 0x00) // VGA-compatible controller
    , m_irqHandler(irqHandler)
{
    nv2a::PCIConfigReader readPCIConfig = [&](uint8_t addr) -> uint32_t { return Read32(m_configSpace, addr); };
    nv2a::PCIConfigWriter writePCIConfig = [&](uint8_t addr, uint32_t value) { Write32(m_configSpace, addr, value); };
    nv2a::IRQHandlerFunc handleIRQ = [&](bool level) { irqHandler.HandleIRQ(Read8(m_configSpace, PCI_INTERRUPT_LINE), level); };
    m_nv2a = std::make_unique<nv2a::NV2A>(pSystemRAM, systemRAMSize, readPCIConfig, writePCIConfig, handleIRQ);
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
        *value = m_nv2a->Read(addr, size);
        //log_spew("NV2ADevice::PCIMMIORead:   bar = %d,  address = 0x%x,  size = %u  ->  0x%x\n", barIndex, addr, size, *value);
    }
    else {
        log_spew("NV2ADevice::PCIMMIORead:   Unimplemented read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
        *value = 0;
    }
}

void NV2ADevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    if (barIndex == 0) {
        //log_spew("NV2ADevice::PCIMMIOWrite:  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
        m_nv2a->Write(addr, value, size);
    }
    else {
        log_spew("NV2ADevice::PCIMMIOWrite:  Unimplemented write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
    }
}

}
