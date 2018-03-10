#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "../basic/mcpx.h"

namespace openxbox {

class MCPXRAMDevice : public PCIDevice {
public:
    // constructor
    MCPXRAMDevice(MCPXRevision revision);

    // PCI Device functions
    void Init();
    void Reset();

    uint32_t IORead(int barIndex, uint32_t port, unsigned size);
    void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size);
    uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
    void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
private:
    MCPXRevision m_revision;
};

}
