#include "ac97.h"
#include "openxbox/log.h"

namespace openxbox {

AC97Device::AC97Device(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
        0x0f, 0x02, 0x00) // Audio controller
{
}

AC97Device::~AC97Device() {
}

// PCI Device functions

void AC97Device::Init() {
	RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0xD000 - 0xD0FF
	RegisterBAR(1, 0x80, PCI_BAR_TYPE_IO); // 0xD200 - 0xD27F
	RegisterBAR(2, 0x1000, PCI_BAR_TYPE_MEMORY); // 0xFEC00000 - 0xFEC00FFF
}

void AC97Device::Reset() {
}

}
