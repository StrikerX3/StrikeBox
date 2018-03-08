#include "defs.h"
#include "mcpx.h"

namespace openxbox {

MCPXRevision MCPXRevisionFromHardwareModel(HardwareModel hardwareModel) {
    switch (hardwareModel) {
    case Revision1_0:
    case Revision1_1:
    case Revision1_2:
    case Revision1_3:
    case Revision1_4:
    case Revision1_5:
    case Revision1_6:
        return MCPXRevision::MCPX_X3;
    case DebugKit:
        // EmuWarning("Guessing MCPXVersion");
        return MCPXRevision::MCPX_X2;
    default:
        // UNREACHABLE(hardwareModel);
        return MCPXRevision::MCPX_X3;
    }
}


MCPXDevice::MCPXDevice(MCPXRevision revision) {
    m_revision = revision;
}

// PCI Device functions

void MCPXDevice::Init() {
    //	m_deviceID = ?;
    //	m_vendorID = PCI_VENDOR_ID_NVIDIA;
}

void MCPXDevice::Reset() {
}

uint32_t MCPXDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    return 0;
}

void MCPXDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
}

uint32_t MCPXDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    return 0;
}

void MCPXDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    // TODO : Log unexpected bar access
}

}
