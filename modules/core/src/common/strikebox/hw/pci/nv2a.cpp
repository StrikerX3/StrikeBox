#include "strikebox/hw/pci/nv2a.h"

#include "strikebox/log.h"

namespace strikebox {

// TODO: implement engines
// Refer to envytools: https://envytools.readthedocs.io/en/latest/index.html
//
//  Engine       Offset      Length    Description
// ----------------------------------------------------------------------------
//  PMC        0x000000      0x1000    Master control
//  PBUS       0x001000      0x1000    Bus control
//  PFIFO      0x002000      0x2000    MMIO and DMA FIFO submission to PGRAPH  (there's no VPE in NV2A)
//  PRMA       0x007000      0x1000    Real mode BAR access
//  PVIDEO     0x008000      0x1000    Video overlay
//  PTIMER     0x009000      0x1000    Time measurement and time-based alarms
//  PCOUNTER   0x00A000      0x1000    Performance monitoring counters
//  PMVIO      0x0C0000      0x1000    VGA sequencer and graph controller registers
//  PFB        0x100000      0x1000    Memory interface
//  PSTRAPS    0x101000      0x1000    Straps readout
//  PROM       0x300000     0x20000    ROM access window
//  PGRAPH     0x400000      0x2000    2D/3D graphics engine
//  PCRTC      0x600000      0x1000    CRTC controls
//  PRMCIO     0x601000      0x1000    VGA CRTC and attribute controller registers
//  PRAMDAC    0x680000      0x1000    RAMDAC, video overlay, cursor, and PLL control
//  PRMDIO     0x681000      0x1000    VGA DAC registers
//  PRAMIN     0x700000    0x100000    RAMIN access
//  USER       0x800000    0x200000    PFIFO MMIO/DMA submission area
//

NV2ADevice::NV2ADevice(uint8_t *pSystemRAM, uint32_t systemRAMSize, IRQHandler& irqHandler)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A0, 0xA1,
        0x03, 0x00, 0x00) // VGA-compatible controller
    , m_pSystemRAM(pSystemRAM)
    , m_systemRAMSize(systemRAMSize)
    , m_irqHandler(irqHandler)
{
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
    log_warning("NV2ADevice::IORead:  Unexpected I/O read!   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    *value = 0;
}

void NV2ADevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_warning("NV2ADevice::IOWrite: Unexpected I/O write!  bar = %d,  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);
}

void NV2ADevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    *value = 0;
    log_spew("NV2ADevice::PCIMMIORead:   Unimplemented read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void NV2ADevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("NV2ADevice::PCIMMIOWrite:  Unimplemented write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
