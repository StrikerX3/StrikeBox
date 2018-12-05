#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "../basic/mcpx.h"

namespace vixen {

class MCPXRAMDevice : public PCIDevice {
public:
    // constructor
    MCPXRAMDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID, MCPXRevision revision);
    virtual ~MCPXRAMDevice();

    // PCI Device functions
    void Init();
    void Reset();
private:
    MCPXRevision m_revision;
};

}
