#include "ide.h"
#include "openxbox/log.h"

namespace openxbox {

IDEDevice::IDEDevice()
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01BC, 0xD2,
		0x01, 0x01, 0x8A) // IDE controller
{
}

// PCI Device functions

void IDEDevice::Init() {
    RegisterBAR(0, 16, PCI_BAR_TYPE_IO); // 0xFF60 - 0xFF6F
}

void IDEDevice::Reset() {
}

uint32_t IDEDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("IDEDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
    
    return 0;
}

void IDEDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("IDEDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t IDEDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("IDEDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void IDEDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("IDEDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
