#include "agpbridge.h"
#include "openxbox/log.h"

namespace openxbox {

AGPBridgeDevice::AGPBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
	: PCIBridgeDevice(vendorID, deviceID, revisionID)
{
}

// PCI Device functions

void AGPBridgeDevice::Init() {
    Write16(m_configSpace, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_TYPE_32);
    Write16(m_configSpace, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_TYPE_32);
    PCIBridgeDevice::Init();
}

}
