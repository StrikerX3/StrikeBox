#include "usb_pci.h"
#include "openxbox/log.h"

namespace openxbox {

USBPCIDevice::USBPCIDevice()
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A5, 0xA1,
		0x0c, 0x03, 0x10) // USB OHCI
{
}

// PCI Device functions

void USBPCIDevice::Init() {
    RegisterBAR(0, 4096, PCI_BAR_TYPE_MEMORY); // 0xFED00000 - 0xFED00FFF  and  0xFED08000 - 0xFED08FFF
}

void USBPCIDevice::Reset() {
}

uint32_t USBPCIDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    //log_spew("USBPCIDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
	log_warning("USBPCIDevice::IORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    return 0;
}

void USBPCIDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    //log_spew("USBPCIDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
	log_warning("USBPCIDevice::IOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t USBPCIDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    //log_spew("USBPCIDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO
	log_warning("USBPCIDevice::MMIORead:  Unimplemented!  bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    return 0;
}

void USBPCIDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    //log_spew("USBPCIDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
	log_warning("USBPCIDevice::MMIOWrite: Unimplemented!  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
}

}
