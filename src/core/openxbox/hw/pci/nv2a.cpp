#include "nv2a.h"
#include "openxbox/log.h"

namespace openxbox {

NV2ADevice::NV2ADevice()
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A0, 0xA1,
		0x03, 0x00, 0x00) // VGA-compatible controller
{
}

// PCI Device functions

void NV2ADevice::Init() {
	RegisterBAR(0, 16 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0xFD000000 - 0xFDFFFFFF
	RegisterBAR(1, 128 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0xF0000000 - 0xF7FFFFFF
	// TODO: check if this is correct
	RegisterBAR(2, 64 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0xF8000000 - 0xFBFFFFFF
}

void NV2ADevice::Reset() {
}

uint32_t NV2ADevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("NV2ADevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
    
    return 0;
}

void NV2ADevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("NV2ADevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t NV2ADevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("NV2ADevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void NV2ADevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("NV2ADevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
