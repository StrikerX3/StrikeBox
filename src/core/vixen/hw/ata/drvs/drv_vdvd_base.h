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

#include "ata_device_driver.h"

namespace vixen {
namespace hw {
namespace ata {

/*!
 * Base class for virtual DVD drive ATA device drivers.
 */
class BaseDVDDriveATADeviceDriver : public IATADeviceDriver {
public:
    BaseDVDDriveATADeviceDriver();
    virtual ~BaseDVDDriveATADeviceDriver() override;

    // ----- ATA commands -----------------------------------------------------

    void IdentifyDevice(IdentifyDeviceData *data) override;
    bool IdentifyPACKETDevice(IdentifyPACKETDeviceData *data) override;
    bool SecurityUnlock(uint8_t unlockData[kSectorSize]) override;
    bool SetDeviceParameters(uint8_t heads, uint8_t sectorsPerTrack) override;
    
    // ----- Feature sets -----------------------------------------------------

    bool SupportsPACKETCommands() override { return true; }
    bool SupportsOverlap() override { return false; }

    // ----- Sector access ----------------------------------------------------
    
    virtual bool ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override = 0;
    virtual bool WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override = 0;
    
    // ----- Utility functions ------------------------------------------------
    
    bool IsAttached() override { return true; }
    bool IsLBAAddressUserAccessible(uint32_t lbaAddress) override;
    uint32_t CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) override;
    void LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) override;
    uint8_t GetPacketTransferSize() override;

protected:
};

}
}
}
