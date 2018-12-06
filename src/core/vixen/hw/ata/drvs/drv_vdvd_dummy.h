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

#include "drv_vdvd_base.h"

namespace vixen {
namespace hw {
namespace ata {

/*!
 * This dummy ATA device driver represents a basic DVD drive that does not contain media.
 */
class DummyDVDDriveATADeviceDriver : public BaseDVDDriveATADeviceDriver {
public:
    DummyDVDDriveATADeviceDriver();
    ~DummyDVDDriveATADeviceDriver() override;

    // ----- Sector access ----------------------------------------------------
    
    bool ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override;
    bool WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override;
};

}
}
}
