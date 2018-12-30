#include "mcpx_ram.h"
#include "vixen/log.h"

namespace vixen {

MCPXRAMDevice::MCPXRAMDevice(MCPXRevision revision)
    : PCIDevice(PCI_HEADER_TYPE_MULTIFUNCTION, PCI_VENDOR_ID_NVIDIA, 0x02A6, 0xA1,
        0x05, 0x00, 0x00) // RAM controller
{
    m_revision = revision;
}

MCPXRAMDevice::~MCPXRAMDevice() {
}

// PCI Device functions

void MCPXRAMDevice::Init() {
    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_66MHZ);

    // Unknown registers
    Write32(m_configSpace, 0x64, 0x1208001);
    Write32(m_configSpace, 0x68, 0x6c);
}

void MCPXRAMDevice::Reset() {
}

}
