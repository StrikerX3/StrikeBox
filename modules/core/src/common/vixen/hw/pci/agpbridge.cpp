#include "vixen/hw/pci/agpbridge.h"

#include "vixen/log.h"

namespace vixen {

AGPBridgeDevice::AGPBridgeDevice()
    : PCIBridgeDevice(PCI_VENDOR_ID_NVIDIA, 0x01B7, 0xA1)
{
}

AGPBridgeDevice::~AGPBridgeDevice() {
}

// PCI Device functions

void AGPBridgeDevice::Init() {
    Write16(m_configSpace, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_TYPE_32);
    Write16(m_configSpace, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_TYPE_32);
    PCIBridgeDevice::Init();

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_66MHZ | PCI_STATUS_DEVSEL_MEDIUM);
    Write16(m_configSpace, PCI_SEC_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_DEVSEL_MEDIUM);
    Write8(m_configSpace, PCI_MIN_GNT, 0x80);
    Write8(m_configSpace, PCI_MAX_LAT, 0x00);
    Write8(m_configSpace, PCI_IO_BASE, 0xf0);
    Write8(m_configSpace, PCI_IO_LIMIT, 0x0);
    Write16(m_configSpace, PCI_MEMORY_BASE, 0xfd00);
    Write16(m_configSpace, PCI_MEMORY_LIMIT, 0xfe70);
    Write16(m_configSpace, PCI_PREF_MEMORY_BASE, 0xf000);
    Write16(m_configSpace, PCI_PREF_MEMORY_LIMIT, 0xf3f0);
    Write32(m_configSpace, PCI_PREF_BASE_UPPER32, 0xff3fbfff);
    Write32(m_configSpace, PCI_PREF_LIMIT_UPPER32, 0xafff7fff);
    Write16(m_configSpace, PCI_IO_BASE_UPPER16, 0xf5fd);
    Write16(m_configSpace, PCI_IO_LIMIT_UPPER16, 0x3eff);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x0);

    // Unknown registers
    Write16(m_configSpace, 0x44, 0x0);
    Write16(m_configSpace, 0x46, 0x8000);
    Write16(m_configSpace, 0x48, 0x14);
    Write16(m_configSpace, 0x4c, 0x1);
    Write16(m_configSpace, 0x50, 0x0);
    Write32(m_configSpace, 0x54, 0x10000000);
    for (uint8_t i = 0; i < 0x100 - 0x58; i += 4) {
        Write32(m_configSpace, 0x58 + i, 0xffffffff);
    }
}

}
