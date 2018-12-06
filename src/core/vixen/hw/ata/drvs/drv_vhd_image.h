// ATA/ATAPI-4 emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement a subset of the ATA/ATAPI-4 specification
// that satisifies the requirements of an IDE interface for the Original Xbox.
//
// Specification:
// http://www.t13.org/documents/UploadedDocuments/project/d1153r18-ATA-ATAPI-4.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification.
#pragma once

#include <cstdint>

#include "drv_vhd_base.h"

namespace vixen {
namespace hw {
namespace ata {

/*!
 * A virtual hard disk ATA device driver based on an image file.
 *
 * It can read/write directly to the image file or use write-on-copy, in which
 * case all writes done on a temporary file and subsequent reads to overwritten
 * sectors are redirected to the temporary file.
 */
class ImageHardDriveATADeviceDriver : public BaseHardDriveATADeviceDriver {
public:
    ImageHardDriveATADeviceDriver();
    ~ImageHardDriveATADeviceDriver() override;

    // ----- Virtual hard disk image initialization ---------------------------

    bool LoadImageFile(const char *imagePath, bool copyOnWrite);

    // ----- Sector access ----------------------------------------------------
    
    bool ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override;
    bool WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override;

private:
    FILE *m_fpImage = NULL;
    bool m_copyOnWrite;
};

}
}
}
