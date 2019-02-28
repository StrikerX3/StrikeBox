#include "strikebox/hw/basic/mcpx.h"

#include "strikebox/hw/defs.h"

namespace strikebox {

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

}
