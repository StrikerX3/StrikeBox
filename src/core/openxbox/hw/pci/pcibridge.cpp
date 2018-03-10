#include "pcibridge.h"
#include "openxbox/log.h"

namespace openxbox {

PCIBridgeDevice::PCIBridgeDevice()
	: PCIDevice(PCI_HEADER_TYPE_BRIDGE, PCI_VENDOR_ID_NVIDIA, 0x01B8, 0xD2,
		0x06, 0x04, 0x00, // PCI bridge
		/*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
}

// PCI Device functions

void PCIBridgeDevice::Init() {
}

void PCIBridgeDevice::Reset() {
}

uint32_t PCIBridgeDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("PCIBridgeDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
    
    return 0;
}

void PCIBridgeDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("PCIBridgeDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t PCIBridgeDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("PCIBridgeDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void PCIBridgeDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("PCIBridgeDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
