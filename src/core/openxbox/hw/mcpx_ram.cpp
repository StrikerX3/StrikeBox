#include "defs.h"
#include "mcpx_ram.h"
#include "openxbox/log.h"

namespace openxbox {

MCPXRAMDevice::MCPXRAMDevice(MCPXRevision revision) {
    m_revision = revision;
}

// PCI Device functions

void MCPXRAMDevice::Init() {
    m_deviceID = 0x02a6;
    m_vendorID = PCI_VENDOR_ID_NVIDIA;
}

void MCPXRAMDevice::Reset() {
}

uint32_t MCPXRAMDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_debug("MCPXRAMDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
    return 0;
}

void MCPXRAMDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_debug("MCPXRAMDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t MCPXRAMDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_debug("MCPXRAMDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);
    return 0;
}

void MCPXRAMDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_debug("MCPXRAMDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
    // TODO : Log unexpected bar access
}

}
