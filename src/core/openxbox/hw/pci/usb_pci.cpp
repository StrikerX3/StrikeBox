#include "usb_pci.h"
#include "openxbox/log.h"

namespace openxbox {

USBPCIDevice::USBPCIDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
        0x0c, 0x03, 0x10) // USB OHCI
{
}

USBPCIDevice::~USBPCIDevice() {
}

// PCI Device functions

void USBPCIDevice::Init() {
    RegisterBAR(0, 4096, PCI_BAR_TYPE_MEMORY); // 0xFED00000 - 0xFED00FFF  and  0xFED08000 - 0xFED08FFF
}

void USBPCIDevice::Reset() {
}

}
