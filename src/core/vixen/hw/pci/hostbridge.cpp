#include "hostbridge.h"
#include "vixen/log.h"

namespace vixen {

HostBridgeDevice::HostBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
    : PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
        0x06, 0x00, 0x00, // Host bridge
        /*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
    Write8(m_configSpace, PCI_INTERRUPT_PIN, 0x1);
    Write8(m_configSpace, 0x87, 3);
}

HostBridgeDevice::~HostBridgeDevice() {
}

// PCI Device functions

void HostBridgeDevice::Init() {
    RegisterBAR(0, 1024 * 1024 * 1024, PCI_BAR_TYPE_MEMORY); // 0x40000000 - 0x7FFFFFFF
}

void HostBridgeDevice::Reset() {
}

void HostBridgeDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    log_spew("HostBridgeDevice::PCIMMIORead:   Unimplemented!  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    
    *value = 0;
}

void HostBridgeDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("HostBridgeDevice::PCIMMIOWrite:  Unimplemented!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
