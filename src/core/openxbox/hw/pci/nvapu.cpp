#include "nvapu.h"
#include "openxbox/log.h"

namespace openxbox {

NVAPUDevice::NVAPUDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
		0x0f, 0x02, 0x00) // Audio controller
{
}

// PCI Device functions

void NVAPUDevice::Init() {
    RegisterBAR(0, 512 * 1024, PCI_BAR_TYPE_MEMORY); // 0xFE800000 - 0xFE87FFFF
}

void NVAPUDevice::Reset() {
}

uint32_t NVAPUDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    //log_spew("NVAPUDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
	log_warning("NVAPUDevice::IORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    return 0;
}

void NVAPUDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    //log_spew("NVAPUDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
	log_warning("NVAPUDevice::IOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t NVAPUDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    //log_spew("NVAPUDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO
	log_warning("NVAPUDevice::MMIORead:  Unimplemented!  bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    return 0;
}

void NVAPUDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    //log_spew("NVAPUDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
	log_warning("NVAPUDevice::MMIOWrite: Unimplemented!  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
}

}
