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
 * Base class for virtual hard drive ATA device drivers.
 */
class BaseHardDriveATADeviceDriver : public IATADeviceDriver {
public:
    BaseHardDriveATADeviceDriver();
    virtual ~BaseHardDriveATADeviceDriver() override;

    // ----- ATA commands -----------------------------------------------------

    void IdentifyDevice(IdentifyDeviceData *data) override;
    bool IdentifyPacketDevice(IdentifyPacketDeviceData *data) override;
    bool SecurityUnlock(uint8_t unlockData[kSectorSize]) override;
    bool SetDeviceParameters(uint8_t heads, uint8_t sectorsPerTrack) override;
    
    // ----- Feature sets -----------------------------------------------------

    bool SupportsPacketCommands() override { return false; }
    bool SupportsOverlap() override { return false; }
    bool IsOverlapEnabled() override { return false; }

    // ----- Data access ------------------------------------------------------
    
    virtual bool Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) override = 0;
    virtual bool Write(uint64_t byteAddress, uint8_t *buffer, uint32_t size) override = 0;
    
    // ----- Utility functions ------------------------------------------------
    
    bool IsAttached() override { return true; }
    bool IsLBAAddressUserAccessible(uint32_t lbaAddress) override;
    uint32_t CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) override;
    void LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) override;
    uint8_t GetPacketTransferSize() override { return 0; }

protected:
    uint32_t m_sectorCapacity;

    uint16_t m_numCylinders;
    uint8_t m_numHeadsPerCylinder;
    uint8_t m_numSectorsPerTrack;

    uint16_t m_numLogicalCylinders;
    uint8_t m_numLogicalHeads;
    uint8_t m_numLogicalSectorsPerTrack;

    bool m_locked;
};

}
}
}
