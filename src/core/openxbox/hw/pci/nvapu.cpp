#include "nvapu.h"
#include "openxbox/log.h"

namespace openxbox {

NVAPUDevice::NVAPUDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
        0x0f, 0x02, 0x00) // Audio controller
{
}

NVAPUDevice::~NVAPUDevice() {
}

// PCI Device functions

void NVAPUDevice::Init() {
    RegisterBAR(0, 512 * 1024, PCI_BAR_TYPE_MEMORY); // 0xFE800000 - 0xFE87FFFF
}

void NVAPUDevice::Reset() {
}

void NVAPUDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    log_spew("NVAPUDevice::PCIMMIORead:   Unimplemented!  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void NVAPUDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::PCIMMIOWrite:  Unimplemented!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
