#include "mcpx_ram.h"
#include "vixen/log.h"

namespace vixen {

MCPXRAMDevice::MCPXRAMDevice(MCPXRevision revision)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A6, 0xA1,
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
