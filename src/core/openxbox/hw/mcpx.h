#pragma once

#include <cstdint>

#include "defs.h"
#include "pci.h"

namespace openxbox {

// MCPX ROM versions.
typedef enum {
    MCPX_1_0,
    MCPX_1_1,
} MCPXROMVersion;

// MCPX revisions.
typedef enum {
    MCPX_X2,
    MCPX_X3,
} MCPXRevision;


MCPXRevision MCPXRevisionFromHardwareModel(HardwareModel hardwareModel);


class MCPXDevice : public PCIDevice {
public:
    // constructor
    MCPXDevice(MCPXRevision revision);

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
