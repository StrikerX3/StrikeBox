#include "agpbridge.h"
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
}

}
