#pragma once

namespace vixen {

// The following physical memory ranges are reserved on all systems:
// 0x00000000 - 0x00000003 :: D3D push buffer address
// 0x0000f000 - 0x0000ffff :: Page directory
//
// On 64 MiB systems, the following ranges are also reserved:
// 0x03fe0000 - 0x03feffff :: NV2A instance memory
// 0x03ff0000 - 0x03ffffff :: PFN database
//
// On 128 MiB systems, the following ranges are also reserved:
// 0x07fd0000 - 0x07feffff :: PFN database
// 0x07ff0000 - 0x07ffffff :: NV2A instance memry
//
// The kernel image is decrypted and extracted at physical address 0x10000 and
// manually mapped to virtual address 0x80010000.

#define XBOX_RAM_SIZE_RETAIL  (64 * MiB)
#define XBOX_RAM_SIZE_DEBUG   (128 * MiB)
#define XBOX_ROM_AREA_SIZE    (16 * MiB)

}
