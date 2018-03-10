#pragma once

#include <cstdint>

#include "../defs.h"

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

}
