#include "mcpx_ram.h"
#include "vixen/log.h"

namespace vixen {

MCPXRAMDevice::MCPXRAMDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID, MCPXRevision revision)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, vendorID, deviceID, revisionID,
        0x05, 0x00, 0x00) // RAM controller
{
    m_revision = revision;
}

MCPXRAMDevice::~MCPXRAMDevice() {
}

// PCI Device functions

void MCPXRAMDevice::Init() {
}

void MCPXRAMDevice::Reset() {
}

}
