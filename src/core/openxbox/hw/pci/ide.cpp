#include "ide.h"
#include "openxbox/log.h"

namespace openxbox {

IDEDevice::IDEDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
        0x01, 0x01, 0x8A) // IDE controller
{
}

IDEDevice::~IDEDevice() {
}

// PCI Device functions

void IDEDevice::Init() {
    RegisterBAR(4, 16, PCI_BAR_TYPE_IO); // 0xFF60 - 0xFF6F
}

void IDEDevice::Reset() {
}

void IDEDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("IDEDevice::PCIIORead:   Unimplemented!  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
}

void IDEDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_spew("IDEDevice::PCIIOWrite:  Unimplemented!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

}
