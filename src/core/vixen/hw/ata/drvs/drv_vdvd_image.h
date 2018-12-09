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
 * A virtual DVD ATA device driver based on an image file.
 *
 * It can read/write directly to the image file or use write-on-copy, in which
 * case all writes done on a temporary file and subsequent reads to overwritten
 * sectors are redirected to the temporary file.
 */
class ImageDVDDriveATADeviceDriver : public BaseDVDDriveATADeviceDriver {
public:
    ImageDVDDriveATADeviceDriver();
    ~ImageDVDDriveATADeviceDriver() override;

    // ----- Virtual DVD image management -------------------------------------

    bool LoadImageFile(const char *imagePath, bool copyOnWrite);
    bool EjectMedia();

    // ----- ATAPI ------------------------------------------------------------

    bool ValidateATAPIPacket(atapi::PacketInformation& packetInfo) override;
    bool ProcessATAPIPacketNonData(atapi::PacketInformation& packetInfo) override;
    bool ProcessATAPIPacketDataRead(atapi::PacketInformation& packetInfo, uint8_t* packetDataBuffer, uint16_t byteCountLimit, uint32_t *packetDataSize) override;
    bool ProcessATAPIPacketDataWrite(atapi::PacketInformation& packetInfo, uint8_t* packetDataBuffer, uint16_t byteCountLimit) override;
    
private:
    FILE *m_fpImage = NULL;
    bool m_copyOnWrite;

    inline bool HasMedia() { return m_fpImage != NULL; }

    uint64_t m_sectorCapacity;

    // ----- Transfer state ---------------------------------------------------

    bool m_transfer = false;
    uint64_t m_currentByte;
    uint64_t m_lastByte;
};

}
}
}
