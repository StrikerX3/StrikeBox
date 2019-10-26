#include "strikebox/hw/pci/usb_pci.h"

#include "strikebox/log.h"

namespace strikebox {

USBPCIDevice::USBPCIDevice(uint8_t irqn, virt86::VirtualProcessor& vp)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01C2, 0xB1,
        0x0c, 0x03, 0x10) // USB OHCI
    , m_irqn(irqn)
    , m_vp(vp)
{
}

USBPCIDevice::~USBPCIDevice() {
}

// PCI Device functions

void USBPCIDevice::Init() {
    RegisterBAR(0, 0x1000, PCI_BAR_TYPE_MEMORY); // 0xFED00000 - 0xFED00FFF  and  0xFED08000 - 0xFED08FFF

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x03);
    Write8(m_configSpace, PCI_MAX_LAT, 0x01);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0xda02);
    Write32(m_configSpace, 0x4c, 0x2);
}

void USBPCIDevice::Reset() {
}

void USBPCIDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    //log_spew("USBPCIDevice::PCIMMIORead:   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    if (barIndex != 0) {
        log_spew("USBPCIDevice::PCIMMIORead:   Unhandled BAR access: %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
        *value = 0;
        return;
    }

    *value = 0;
    log_spew("USBPCIDevice::PCIMMIORead:   Unimplemented read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void USBPCIDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    //log_spew("USBPCIDevice::PCIMMIOWrite:  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
    if (barIndex != 0) {
        log_spew("USBPCIDevice::PCIMMIOWrite:  Unhandled BAR access: %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
        return;
    }

    log_spew("USBPCIDevice::PCIMMIOWrite:  Unimplemented write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
