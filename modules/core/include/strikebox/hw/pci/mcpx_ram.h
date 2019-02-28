#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "../basic/mcpx.h"

namespace strikebox {

class MCPXRAMDevice : public PCIDevice {
public:
    // constructor
    MCPXRAMDevice(MCPXRevision revision);
    virtual ~MCPXRAMDevice();

    // PCI Device functions
    void Init();
    void Reset();
private:
    MCPXRevision m_revision;
};

}
