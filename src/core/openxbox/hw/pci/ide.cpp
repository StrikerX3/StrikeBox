#include "ide.h"
#include "openxbox/log.h"

namespace openxbox {

IDEDevice::IDEDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
		0x01, 0x01, 0x8A) // IDE controller
{
}

// PCI Device functions

void IDEDevice::Init() {
    RegisterBAR(0, 16, PCI_BAR_TYPE_IO); // 0xFF60 - 0xFF6F
}

void IDEDevice::Reset() {
}

}
