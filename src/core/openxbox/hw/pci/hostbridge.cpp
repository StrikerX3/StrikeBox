#include "hostbridge.h"
#include "openxbox/log.h"

namespace openxbox {

HostBridgeDevice::HostBridgeDevice()
	: PCIDevice(PCI_HEADER_TYPE_BRIDGE, PCI_VENDOR_ID_NVIDIA, 0x02A5, 0xA1,
		0x06, 0x00, 0x00, // Host bridge
		/*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
}

// PCI Device functions

void HostBridgeDevice::Init() {
    RegisterBAR(0, 1024 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0x40000000 - 0x7FFFFFFF
}

void HostBridgeDevice::Reset() {
}

uint32_t HostBridgeDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("HostBridgeDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
    
    return 0;
}

void HostBridgeDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("HostBridgeDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t HostBridgeDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("HostBridgeDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void HostBridgeDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("HostBridgeDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
