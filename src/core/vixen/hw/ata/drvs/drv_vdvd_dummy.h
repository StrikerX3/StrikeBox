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

    // ----- Data access ------------------------------------------------------
    
    bool Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) override;
    bool Write(uint64_t byteAddress, uint8_t *buffer, uint32_t size) override;

    // ----- ATAPI ------------------------------------------------------------

    bool IdentifyATAPIPacket(uint8_t *packet, atapi::PacketInformation& packetInfo) override;
    bool ProcessATAPIPacketNonData(atapi::PacketInformation& packetInfo) override;
    bool ProcessATAPIPacketDataRead(atapi::PacketInformation& packetInfo, uint8_t* packetDataBuffer, uint16_t byteCountLimit, uint32_t *packetDataSize) override;
    bool ProcessATAPIPacketDataWrite(atapi::PacketInformation& packetInfo, uint8_t* packetDataBuffer, uint16_t byteCountLimit) override;
};

}
}
}
