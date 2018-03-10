#include "defs.h"
#include "mcpx_ram.h"
#include "openxbox/log.h"

namespace openxbox {

MCPXRAMDevice::MCPXRAMDevice(MCPXRevision revision) {
    m_revision = revision;
}

// PCI Device functions

void MCPXRAMDevice::Init() {
    WriteConfigRegister8(PCI_CONFIG_HEADER_TYPE, PCI_TYPE_DEVICE);
    WriteConfigRegister16(PCI_CONFIG_VENDOR_ID, PCI_VENDOR_ID_NVIDIA);
    WriteConfigRegister16(PCI_CONFIG_DEVICE_ID, 0x02A6);
}

void MCPXRAMDevice::Reset() {
}

uint32_t MCPXRAMDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("MCPXRAMDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
    return 0;
}

void MCPXRAMDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("MCPXRAMDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t MCPXRAMDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("MCPXRAMDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);
    return 0;
}

void MCPXRAMDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("MCPXRAMDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
}

}
