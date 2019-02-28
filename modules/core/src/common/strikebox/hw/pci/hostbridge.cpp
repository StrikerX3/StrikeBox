#include "strikebox/hw/pci/hostbridge.h"

#include "strikebox/log.h"

namespace strikebox {

HostBridgeDevice::HostBridgeDevice()
    : PCIDevice(PCI_HEADER_TYPE_MULTIFUNCTION, PCI_VENDOR_ID_NVIDIA, 0x02A5, 0xA1,
        0x06, 0x00, 0x00, // Host bridge
        /*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
}

HostBridgeDevice::~HostBridgeDevice() {
}

// PCI Device functions

void HostBridgeDevice::Init() {
    RegisterBAR(0, 1024 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0x40000000 - 0x7FFFFFFF
    Write32(m_configSpace, PCI_BASE_ADDRESS_0, 0x40000008);

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x40);

    // Capability list
    Write8(m_configSpace, 0x40, PCI_CAP_ID_AGP);
    Write8(m_configSpace, 0x41, 0x60);

    Write8(m_configSpace, 0x60, PCI_CAP_ID_HT);
    Write8(m_configSpace, 0x61, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x42, 0x20);
    Write32(m_configSpace, 0x44, 0x1f000217);
    Write8(m_configSpace, 0x4c, 0x1);
    Write8(m_configSpace, 0x57, 0x10);
    Write32(m_configSpace, 0x58, 0xffffffff);
    Write32(m_configSpace, 0x5c, 0xffffffff);
    Write32(m_configSpace, 0x60, 0x20010008);
    Write32(m_configSpace, 0x64, 0x88880120);
    Write32(m_configSpace, 0x68, 0x10);
    Write32(m_configSpace, 0x6c, 0x0f0f0f21);
    Write32(m_configSpace, 0x70, 0xffffffff);
    Write32(m_configSpace, 0x74, 0xffffffff);
    Write32(m_configSpace, 0x78, 0xffffffff);
    Write32(m_configSpace, 0x7c, 0xffffffff);
    Write32(m_configSpace, 0x87, 3);
    Write32(m_configSpace, 0x88, 0x1);
    Write32(m_configSpace, 0x8c, 0x3ff6f417);
    Write32(m_configSpace, 0x94, 0xf9feffff);
    Write32(m_configSpace, 0xa4, 0x2001);
    Write32(m_configSpace, 0xb0, 0x1);
    Write32(m_configSpace, 0xc0, 0x33333);
    Write32(m_configSpace, 0xc4, 0x33333);
    Write32(m_configSpace, 0xc8, 0x13);
    Write32(m_configSpace, 0xd4, 0x1);
    Write32(m_configSpace, 0xd8, 0x7f0ffff);
    Write32(m_configSpace, 0xe0, 0x400006);
    Write32(m_configSpace, 0xe4, 0x1ff75b7);
    Write32(m_configSpace, 0xf0, 0xf0000001);
}

void HostBridgeDevice::Reset() {
}

void HostBridgeDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    log_spew("HostBridgeDevice::PCIMMIORead:   Unimplemented!  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    
    *value = 0;
}

void HostBridgeDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("HostBridgeDevice::PCIMMIOWrite:  Unimplemented!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
