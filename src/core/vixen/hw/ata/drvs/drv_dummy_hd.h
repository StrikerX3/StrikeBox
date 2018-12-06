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
 * The dummy ATA device driver represents a basic hard drive that is filled with zeros.
 */
class DummyHardDriveATADeviceDriver : public IATADeviceDriver {
public:
    DummyHardDriveATADeviceDriver();
    ~DummyHardDriveATADeviceDriver() override;
 
    // ----- ATA commands -----------------------------------------------------

    void IdentifyDevice(IdentifyDeviceData *data) override;
    
    // ----- Sector access ----------------------------------------------------
    
    bool ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override;
    bool WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) override;
    
    // ----- Utility functions ------------------------------------------------
    
    bool IsAttached() override { return true; }
    bool IsLBAAddressUserAccessible(uint32_t lbaAddress) override;
    uint32_t CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) override;
    void LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) override;

private:
    uint16_t m_numCylinders;
    uint8_t m_numHeadsPerCylinder;
    uint8_t m_numSectorsPerTrack;
};

extern DummyHardDriveATADeviceDriver g_dummyATADeviceDriver;

}
}
}
