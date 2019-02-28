#include "strikebox/hw/pci/ac97.h"

#include "strikebox/log.h"

namespace strikebox {

AC97Device::AC97Device()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B1, 0xB1,
        0x04, 0x01, 0x00) // Multimedia Audio Controller
{
}

AC97Device::~AC97Device() {
}

// PCI Device functions

void AC97Device::Init() {
	RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0xD000 - 0xD0FF
	RegisterBAR(1, 0x80, PCI_BAR_TYPE_IO); // 0xD200 - 0xD27F
	RegisterBAR(2, 0x1000, PCI_BAR_TYPE_MEMORY); // 0xFEC00000 - 0xFEC00FFF

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x02);
    Write8(m_configSpace, PCI_MAX_LAT, 0x05);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x2);
    Write16(m_configSpace, 0x4c, 0x106);
}

void AC97Device::Reset() {
}

void AC97Device::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    //log_spew("AC97Device::PCIIORead:   Unimplemented!  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
}

void AC97Device::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    //log_spew("AC97Device::PCIIOWrite:  Unimplemented!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

void AC97Device::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    //log_spew("AC97Device::PCIMMIORead:   Unimplemented!  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void AC97Device::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    //log_spew("AC97Device::PCIMMIOWrite:  Unimplemented!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
