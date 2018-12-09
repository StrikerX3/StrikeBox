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
#include "drv_vdvd_image.h"

#include "vixen/log.h"
#include "vixen/io.h"
#include "vixen/hw/ata/atapi_defs.h"
#include "vixen/hw/ata/atapi_xbox.h"
#include "vixen/hw/ata/atapi_utils.h"

namespace vixen {
namespace hw {
namespace ata {

using namespace atapi;

ImageDVDDriveATADeviceDriver::ImageDVDDriveATADeviceDriver() {
    strcpy(m_serialNumber, "9876543210");
    strcpy(m_firmwareRevision, "1.00");
    strcpy(m_modelNumber, "IMAGE DVD 12345");
}

ImageDVDDriveATADeviceDriver::~ImageDVDDriveATADeviceDriver() {
}

bool ImageDVDDriveATADeviceDriver::LoadImageFile(const char *imagePath, bool copyOnWrite) {
    // TODO: Refactor image management into a class hierarchy:
    // IDiskImageProvider  <<interface>>
    //   XISODiskImageProvider
    //   ...

    // TODO: Use memory-mapped I/O

    // Try to load the image file
    m_fpImage = NULL;
    auto err = fopen_s(&m_fpImage, imagePath, "rb");
    if (err) {
        log_fatal("ImageDVDDriveATADeviceDriver::LoadImage:  Could not open image \"%s\": error code 0x%x\n", imagePath, err);
        return false;
    }

    // Determine image file size
    fseek(m_fpImage, 0, SEEK_END);
    uint64_t imageSize = _ftelli64(m_fpImage);
    uint64_t imageSizeInSectors = imageSize / kDVDSectorSize;
    log_info("ImageDVDDriveATADeviceDriver::LoadImage:  Loaded image \"%s\": %llu bytes -> %llu sectors\n", imagePath, imageSize, imageSizeInSectors);
    if (imageSizeInSectors > kMaxSectorsDVDDualLayer) {
        log_warning("ImageDVDDriveATADeviceDriver::LoadImage:  Image is too big; limiting to the first %u sectors\n", kMaxSectorsDVDDualLayer);
        imageSizeInSectors = kMaxSectorsDVDDualLayer;
    }

    m_sectorCapacity = imageSizeInSectors;

    return true;
}

bool ImageDVDDriveATADeviceDriver::EjectMedia() {
    if (m_fpImage == NULL) {
        log_warning("ImageDVDDriveATADeviceDriver::EjectMedia:  No media to eject\n");
        return false;
    }

    log_info("ImageDVDDriveATADeviceDriver::EjectMedia:  Media ejected\n");
    fclose(m_fpImage);
    m_fpImage = NULL;
    // TODO: notify media removal
    return true;
}

bool ImageDVDDriveATADeviceDriver::ValidateATAPIPacket(PacketInformation& packetInfo) {
    log_debug("ImageDVDDriveATADeviceDriver::ValidateATAPIPacket:  Operation code 0x%x\n", packetInfo.cdb.opCode.u8, packetInfo.cdb.opCode.fields.commandCode, packetInfo.cdb.opCode.fields.groupCode);
    
    // TODO: device-specific validation
    
    // Check if the command is supported and has valid parameters.
    return ValidateCommand(packetInfo);
}

bool ImageDVDDriveATADeviceDriver::ProcessATAPIPacketNonData(PacketInformation& packetInfo) {
    switch (packetInfo.cdb.opCode.u8) {
    case OpTestUnitReady:
        // If there is no disc in the drive, return the expected sense key and parameters.
        // The default values tell that there is media in the drive and the device is ready to accept commands.
        if (!HasMedia()) {
            packetInfo.result.status = StCheckCondition;
            packetInfo.result.senseKey = SKNotReady;
            packetInfo.result.additionalSenseCode = ASCMediumNotPresent;
        }

        return true;
    default:
        log_debug("ImageDVDDriveATADeviceDriver::ProcessATAPIPacketNonData:  Unimplemented operation code 0x%x\n", packetInfo.cdb.opCode.u8);
        return false;
    }
}

bool ImageDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead(PacketInformation& packetInfo, uint8_t *packetDataBuffer, uint16_t byteCountLimit, uint32_t *packetDataSize) {
    // TODO: refactor command handling
    // - make a structure similar to ATA commands
    switch (packetInfo.cdb.opCode.u8) {
    case OpModeSense10:
        switch (packetInfo.cdb.modeSense10.pageCode) {
        case kPageCodeAuthentication:
        {
            // TODO: handle partial reads (if those ever happen here)
            if (byteCountLimit < sizeof(XboxDVDAuthentication)) {
                packetInfo.result.aborted = true;
                packetInfo.result.deviceFault = true;
                return false;
            }

            // Fill in just enough information to pass basic authentication checks on modified kernels
            // TODO: Research Xbox DVD authentication
            // https://multimedia.cx/eggs/xbox-sphinx-protocol/
            XboxDVDAuthentication *dvdAuth = reinterpret_cast<XboxDVDAuthentication *>(packetDataBuffer);
            dvdAuth->CDFValid = 1;
            dvdAuth->PartitionArea = 1;
            dvdAuth->Authentication = 1;
            
            *packetDataSize = sizeof(XboxDVDAuthentication);
            return true;
        }
        default:
            log_debug("ImageDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead:  Unimplemented page code 0x%x for MODE SENSE(10)\n", packetInfo.cdb.modeSense10.pageCode);
            return false;
        }
    case OpRead10:
    {
        if (HasMedia()) {
            uint32_t lba = B2L32(packetInfo.cdb.read10.lba);
            uint16_t transferLength = B2L16(packetInfo.cdb.read10.length);
            
            packetInfo.transferSize = transferLength * kDVDSectorSize;

            // If this is the first read, fill in transfer data
            if (!m_transfer) {
                m_currentByte = lba * kDVDSectorSize;
                m_lastByte = m_currentByte + transferLength * kDVDSectorSize;
            }

            // TODO: maybe handle caching? Could improve performance if accessing real media on supported drives

            // Read from media
            uint16_t readLen = (byteCountLimit < m_lastByte - m_currentByte)
                ? byteCountLimit
                : m_lastByte - m_currentByte;
            _fseeki64(m_fpImage, m_currentByte, SEEK_SET);

            *packetDataSize = fread(packetDataBuffer, 1, readLen, m_fpImage);

            // Update position
            m_currentByte += *packetDataSize;
            if (m_currentByte >= m_lastByte || *packetDataSize < readLen) {
                m_transfer = false;
            }
        }
        else {
            // Say that there is no disc in the drive
            packetInfo.result.status = StCheckCondition;
            packetInfo.result.senseKey = SKNotReady;
            packetInfo.result.additionalSenseCode = ASCMediumNotPresent;
        }

        return true;
    }
    case OpReadCapacity:
    {
        ReadCapacityData *capData = reinterpret_cast<ReadCapacityData *>(packetDataBuffer);
      
        if (HasMedia()) {
            L2B32(capData->lba, m_sectorCapacity);
            L2B32(capData->blockLength, kDVDSectorSize);
        }
        else {
            // Say that there is no disc in the drive
            packetInfo.result.status = StCheckCondition;
            packetInfo.result.senseKey = SKNotReady;
            packetInfo.result.additionalSenseCode = ASCMediumNotPresent;

            L2B32(capData->lba, 0);
            L2B32(capData->blockLength, 0);
        }

        *packetDataSize = sizeof(ReadCapacityData);
        return true;
    }
    default:
        log_debug("ImageDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead:  Unimplemented operation code 0x%x\n", packetInfo.cdb.opCode.u8);
        return false;
    }
}

bool ImageDVDDriveATADeviceDriver::ProcessATAPIPacketDataWrite(PacketInformation& packetInfo, uint8_t *packetDataBuffer, uint16_t byteCountLimit) {

    log_debug("ImageDVDDriveATADeviceDriver::ProcessATAPIPacketDataWrite:  Unimplemented operation code 0x%x\n", packetInfo.cdb.opCode.u8);
    return false;
}

}
}
}
