#include "strikebox/hw/pci/nvapu.h"

#include "strikebox/log.h"

namespace strikebox {

uint32_t GetAPUTime() {
    // This timer counts at 48000 Hz
    auto t = std::chrono::high_resolution_clock::now();
    return static_cast<uint32_t>(t.time_since_epoch().count() * 0.000048000);
}

NVAPUDevice::NVAPUDevice()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B0, 0xB1,
        0x04, 0x01, 0x00) // Multimedia Audio Controller
{
}

NVAPUDevice::~NVAPUDevice() {
}

// PCI Device functions

void NVAPUDevice::Init() {
    RegisterBAR(0, 0x80000, PCI_BAR_TYPE_MEMORY); // 0xFE800000 - 0xFE87FFFF

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x01);
    Write8(m_configSpace, PCI_MAX_LAT, 0x0c);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x2);
    Write32(m_configSpace, 0x4c, 0x50a);
    for (uint8_t i = 0; i < 0x100 - 0x50; i += 4) {
        Write32(m_configSpace, 0x50 + i, 0x20001);
    }
}

void NVAPUDevice::Reset() {
}

void NVAPUDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("NVAPUDevice::PCIIORead:   Unimplemented read!   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    *value = 0;
}

void NVAPUDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::PCIIOWrite:  Unimplemented write!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

void NVAPUDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    // FIXME: this satisfies the most basic needs of the guest software
    if (addr == 0x200C) {
        *value = GetAPUTime();
        log_spew("NVAPUDevice::PCIMMIORead:   Reading dummy value!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    }
    // HACK: Pretend the FIFO is always empty, bypasses hangs when APU isn't fully implemented
    else if (addr == 0x20010) {
        *value = 0x80;
        log_spew("NVAPUDevice::PCIMMIORead:   Reading dummy value!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    }
    else {
        *value = 0;
        log_spew("NVAPUDevice::PCIMMIORead:   Unimplemented read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    }
}

void NVAPUDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::PCIMMIOWrite:  Unimplemented write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
