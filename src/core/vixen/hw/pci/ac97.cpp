#include "ac97.h"
#include "vixen/log.h"

namespace vixen {

AC97Device::AC97Device()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B1, 0xD2,
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

void AC97Device::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("AC97Device::PCIIORead:   Unimplemented!  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
}

void AC97Device::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_spew("AC97Device::PCIIOWrite:  Unimplemented!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

void AC97Device::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    log_spew("AC97Device::PCIMMIORead:   Unimplemented!  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void AC97Device::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("AC97Device::PCIMMIOWrite:  Unimplemented!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
