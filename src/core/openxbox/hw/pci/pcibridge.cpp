#include "pcibridge.h"
#include "openxbox/log.h"
#include "../utils.h"

namespace openxbox {

PCIBridgeDevice::PCIBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
	: PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
        0x06, 0x04, 0x00, // PCI bridge
		/*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
}

// PCI Device functions

void PCIBridgeDevice::Init() {
    TestAndSet16(m_configSpace, PCI_STATUS, PCI_STATUS_66MHZ | PCI_STATUS_FAST_BACK);
    Write16(m_configSpace, PCI_SEC_STATUS, PCI_STATUS_66MHZ | PCI_STATUS_FAST_BACK);
}

void PCIBridgeDevice::Reset() {
    Write8(m_configSpace, PCI_PRIMARY_BUS, 0);
    Write8(m_configSpace, PCI_SECONDARY_BUS,  0);
    Write8(m_configSpace, PCI_SUBORDINATE_BUS, 0);
    Write8(m_configSpace, PCI_SEC_LATENCY_TIMER, 0);

    TestAndSet8(m_configSpace, PCI_IO_BASE, PCI_IO_RANGE_MASK & 0xff);
    TestAndSet8(m_configSpace, PCI_IO_LIMIT, PCI_IO_RANGE_MASK & 0xff);
    TestAndSet16(m_configSpace, PCI_MEMORY_BASE, PCI_MEMORY_RANGE_MASK & 0xffff);
    TestAndSet16(m_configSpace, PCI_MEMORY_LIMIT, PCI_MEMORY_RANGE_MASK & 0xffff);
    TestAndSet16(m_configSpace, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_MASK & 0xffff);
    TestAndSet16(m_configSpace, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_MASK & 0xffff);
    Write32(m_configSpace, PCI_PREF_BASE_UPPER32, 0);
    Write32(m_configSpace, PCI_PREF_LIMIT_UPPER32, 0);

    Write16(m_configSpace, PCI_BRIDGE_CONTROL, 0);
}

void PCIBridgeDevice::WriteConfig(uint32_t reg, uint32_t value, uint8_t size) {
    uint16_t oldControl = Read16(m_configSpace, PCI_BRIDGE_CONTROL);
    uint16_t newControl;

    PCIDevice::WriteConfig(reg, value, size);

    if (RangesOverlap(reg, size, PCI_COMMAND, 2) ||

        // I/O base/limit
        RangesOverlap(reg, size, PCI_IO_BASE, 2) ||

        // Memory base/limit, prefetchable base/limit and I/O base/limit upper 16
        RangesOverlap(reg, size, PCI_MEMORY_BASE, 20) ||

        // VGA enable
        RangesOverlap(reg, size, PCI_BRIDGE_CONTROL, 2)) {

        // TODO: update mappings
        //pci_bridge_update_mappings(s);
    }

    newControl = Read16(m_configSpace, PCI_BRIDGE_CONTROL);
    if (~oldControl & newControl & PCI_BRIDGE_CTL_BUS_RESET) {
        // TODO: Trigger hot reset on 0->1 transition
        //pci_bus_reset(&s->sec_bus);
    }
}

uint32_t PCIBridgeDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    //log_spew("PCIBridgeDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
	log_warning("PCIBridgeDevice::IORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    return 0;
}

void PCIBridgeDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    //log_spew("PCIBridgeDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
	log_warning("PCIBridgeDevice::IOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t PCIBridgeDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    //log_spew("PCIBridgeDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO
	log_warning("PCIBridgeDevice::MMIORead:  Unimplemented!  bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    return 0;
}

void PCIBridgeDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    //log_spew("PCIBridgeDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
	log_warning("PCIBridgeDevice::MMIOWrite: Unimplemented!  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
}

}
