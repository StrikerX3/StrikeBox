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

#include "../../ata/ata_defs.h"
#include "../../atapi/atapi_common.h"
#include "../ata_common.h"
#include "util.h"

namespace vixen {
namespace hw {
namespace ata {

/*!
 * Interface for an ATA device driver.
 * ATA device drivers act as virtual devices attached to ATA channels.
 * They typically interact with real hardware by accessing a hard drive image,
 * an XISO, a host DVD drive or similar methods.
 */
class IATADeviceDriver {
public:
    virtual ~IATADeviceDriver();

    // ----- ATA commands -----------------------------------------------------

    virtual void IdentifyDevice(IdentifyDeviceData *data) = 0;
    virtual bool IdentifyPacketDevice(IdentifyPacketDeviceData *data) = 0;
    virtual bool SecurityUnlock(uint8_t unlockData[kSectorSize]) = 0;
    virtual bool SetDeviceParameters(uint8_t heads, uint8_t sectorsPerTrack) = 0;

    void SetPIOTransferMode(PIOTransferType type, uint8_t mode);
    void SetDMATransferMode(DMATransferType type, uint8_t mode);

    // ----- Data access ------------------------------------------------------

    virtual bool Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) = 0;
    virtual bool Write(uint64_t byteAddress, uint8_t *buffer, uint32_t size) = 0;

    // ----- Feature sets -----------------------------------------------------

    virtual bool SupportsPacketCommands() = 0;
    virtual bool SupportsOverlap() = 0;
    virtual bool IsOverlapEnabled() = 0;
    
    // ----- Medium -----------------------------------------------------------

    virtual bool HasMedium() = 0;
    virtual uint32_t GetMediumCapacitySectors() = 0;
    virtual uint32_t GetSectorSize() = 0;

    // ----- Utility functions ------------------------------------------------

    virtual bool IsAttached() = 0;
    virtual bool IsLBAAddressUserAccessible(uint32_t lbaAddress) = 0;
    virtual uint32_t CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) = 0;
    virtual void LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) = 0;
    virtual uint8_t GetPacketCommandSize() = 0;

protected:
    // ----- Common data ------------------------------------------------------

    char m_serialNumber[kSerialNumberLength + 1];
    char m_firmwareRevision[kFirmwareRevLength + 1];
    char m_modelNumber[kModelNumberLength + 1];

    // [8.37.10] PIO and DMA modes are separate
    PIOTransferType m_pioTransferType = XferTypePIODefault;
    uint8_t m_pioTransferMode = 0;

    DMATransferType m_dmaTransferType = XferTypeMultiWordDMA;
    uint8_t m_dmaTransferMode = 0;
};

}
}
}
