#include "ac97.h"
#include "openxbox/log.h"

namespace openxbox {

AC97Device::AC97Device()
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B1, 0xD2,
		0x0f, 0x02, 0x00) // Audio controller
{
}

// PCI Device functions

void AC97Device::Init() {
	RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0xD000 - 0xD0FF
	RegisterBAR(1, 0x80, PCI_BAR_TYPE_IO); // 0xD200 - 0xD27F
	RegisterBAR(2, 0x1000, PCI_BAR_TYPE_MEMORY); // 0xFEC00000 - 0xFEC00FFF
}

void AC97Device::Reset() {
}

uint32_t AC97Device::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("AC97Device::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
    
    return 0;
}

void AC97Device::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("AC97Device::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t AC97Device::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("AC97Device::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void AC97Device::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("AC97Device::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
