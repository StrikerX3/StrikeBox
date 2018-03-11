#include "hostbridge.h"
#include "openxbox/log.h"

namespace openxbox {

HostBridgeDevice::HostBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
	: PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
		0x06, 0x00, 0x00, // Host bridge
		/*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
    Write8(m_configSpace, PCI_INTERRUPT_PIN, 0x1);
    Write8(m_configSpace, 0x87, 3);
}

// PCI Device functions

void HostBridgeDevice::Init() {
    RegisterBAR(0, 1024 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0x40000000 - 0x7FFFFFFF
}

void HostBridgeDevice::Reset() {
}

}
