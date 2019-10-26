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
#include "strikebox/hw/ata/drvs/drv_vdvd_image.h"

#include "strikebox/log.h"
#include "strikebox/io.h"
#include "strikebox/hw/atapi/atapi_defs.h"
#include "strikebox/hw/atapi/atapi_xbox.h"
#include "strikebox/hw/atapi/atapi_utils.h"

namespace strikebox {
namespace hw {
namespace ata {

using namespace atapi;

ImageDVDDriveATADeviceDriver::ImageDVDDriveATADeviceDriver() {
    strcpy(m_serialNumber, "9876543210");
    strcpy(m_firmwareRevision, "1.0.0");
    strcpy(m_modelNumber, "SBx VDVDD0010000");
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

bool ImageDVDDriveATADeviceDriver::EjectMedium() {
    if (m_fpImage == NULL) {
        log_warning("ImageDVDDriveATADeviceDriver::EjectMedium:  No medium to eject\n");
        return false;
    }

    log_info("ImageDVDDriveATADeviceDriver::EjectMedium:  Medium ejected\n");
    fclose(m_fpImage);
    m_fpImage = NULL;
    // TODO: should we notify media removal?
    return true;
}

bool ImageDVDDriveATADeviceDriver::Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) {
    // TODO: maybe handle caching? Could improve performance if accessing real media on supported drives
    // Should also honor the cache flags
    // Image not loaded
    if (m_fpImage == NULL) {
        return false;
    }

    // Seek address
    if (_fseeki64(m_fpImage, byteAddress, SEEK_SET)) {
        return false;
    }

    // Read data from image
    // TODO: handle copy-on-write
    // If copy-on-write and the sector is copied, read from copy, otherwise read from image file
    // If not copy-on-write, read from image file directly
    int lenRead = fread(buffer, 1, size, m_fpImage);

    // Read is successful if the full size is read
    return lenRead == size;
}

}
}
}
