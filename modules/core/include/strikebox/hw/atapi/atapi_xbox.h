// ATA/ATAPI-4 emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Reverse-engineered Xbox-specific data structures and definitions.
//
// Sources:
//   [c] Cxbx-Reloaded
//   https://github.com/Cxbx-Reloaded/Cxbx-Reloaded
//
//   [w] XboxDevWiki
//   https://xboxdevwiki.net
#pragma once

#include <cstdint>

namespace strikebox {
namespace hw {
namespace atapi {

// SCSI MODE SENSE and MODE SELECT page code for the security data.
// Contains the XboxDVDAuthentication struct.
// [w] http://xboxdevwiki.net/DVD_Drive
const uint8_t kPageCodeAuthentication = 0x3E;

// The Xbox DVD authentication page data.
// [c] https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/blob/e452d56991c7dce655511c5529f4cf1a6fe98e42/import/OpenXDK/include/xboxkrnl/xboxkrnl.h#L2494
struct XboxDVDAuthentication {
    uint8_t Unknown[2];
    uint8_t PartitionArea;
    uint8_t CDFValid;
    uint8_t Authentication;
    uint8_t Unknown2[3];
    uint8_t Unknown3[3];
};

}
}
}
