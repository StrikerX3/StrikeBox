#include "strikebox/hw/pci/nvnet.h"

#include "strikebox/log.h"

namespace strikebox {

NVNetDevice::NVNetDevice()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01C3, 0xB1,
        0x02, 0x00, 0x00) // Ethernet controller
{
}

NVNetDevice::~NVNetDevice() {
}

// PCI Device functions

void NVNetDevice::Init() {
    RegisterBAR(0, NVNET_SIZE, PCI_BAR_TYPE_MEMORY);  // 0xFEF00000 - 0xFEF003FF
    RegisterBAR(1, 16, PCI_BAR_TYPE_IO); // 0xE000 - 0xE00F

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x01);
    Write8(m_configSpace, PCI_MAX_LAT, 0x14);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0xfe02);
    Write32(m_configSpace, 0x4c, 0x4);
}

void NVNetDevice::Reset() {
}

void NVNetDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    //log_spew("NVNetDevice::PCIIORead:   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    if (barIndex != 1) {
        log_spew("NVNetDevice::PCIIORead:   Unhandled BAR access: %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
        *value = 0;
        return;
    }

    *value = 0;
    log_warning("NVNetDevice::PCIIORead:   Unimplemented read!   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
}

void NVNetDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    //log_spew("NVNetDevice::PCIIOWrite:  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
    if (barIndex != 1) {
        log_spew("NVNetDevice::PCIIOWrite:  Unhandled BAR access: %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
        return;
    }

    log_warning("NVNetDevice::PCIIOWrite:  Unimplemented write!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

void NVNetDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    //log_spew("NVNetDevice::PCIMMIORead:   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    if (barIndex != 0) {
        log_spew("NVNetDevice::PCIMMIORead:   Unhandled BAR access: %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
        *value = 0;
        return;
    }

    *value = 0;
    log_warning("NVNetDevice::PCIMMIORead:   Unimplemented read!   bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void NVNetDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    //log_spew("NVNetDevice::PCIMMIOWrite:  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
    if (barIndex != 0) {
        log_spew("NVNetDevice::PCIMMIOWrite:  Unhandled BAR access: %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
        return;
    }

    log_warning("NVNetDevice::PCIMMIOWrite:  Unimplemented write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
