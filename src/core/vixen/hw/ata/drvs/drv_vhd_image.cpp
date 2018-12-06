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
#include "drv_vhd_image.h"

#include "vixen/log.h"
#include "vixen/io.h"

namespace vixen {
namespace hw {
namespace ata {

static void padString(uint8_t *dest, const char *src, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (*src) {
            dest[i ^ 1] = *src++;
        }
        else {
            dest[i ^ 1] = ' ';
        }
    }
}

ImageHardDriveATADeviceDriver::ImageHardDriveATADeviceDriver() {
    // TODO: fill in with appropriate data
    strcpy(m_serialNumber, "1234567890");
    strcpy(m_firmwareRevision, "1.00");
    strcpy(m_modelNumber, "IMAGE HD 12345");

    // Initialize an empty (invalid) disk
    m_numCylinders = 0;
    m_numHeadsPerCylinder = 0;
    m_numSectorsPerTrack = 0;
    m_sectorCapacity = 0;
}

bool ImageHardDriveATADeviceDriver::LoadImageFile(const char *imagePath, bool copyOnWrite) {
    // TODO: Detect image format; some images may provide CHS parameters
    // NOTE: For now, we're loading RAW images only
    
    // TODO: Refactor image management into a class hierarchy:
    // IDiskImageProvider  <<interface>>
    //   RawDiskImageProvider
    //   Qcow2DiskImageProvider
    //   ...

    // TODO: Use memory-mapped I/O

    // Try to load the image file
    m_fpImage = NULL;
    auto err = fopen_s(&m_fpImage, imagePath, "r+b");
    if (err) {
        log_fatal("ImageHardDriveATADeviceDriver::LoadImage:  Could not open image \"%s\": error code 0x%x\n", imagePath, err);
        return false;
    }

    // Determine image file size
    fseek(m_fpImage, 0, SEEK_END);
    uint64_t imageSize = ftell(m_fpImage);
    uint64_t imageSizeInSectors = imageSize / kSectorSize;
    log_info("ImageHardDriveATADeviceDriver::LoadImage:  Loaded image \"%s\": %llu bytes -> %llu sectors\n", imagePath, imageSize, imageSizeInSectors);
    if (imageSizeInSectors > kMaxLBASectorCapacity) {
        log_warning("ImageHardDriveATADeviceDriver::LoadImage:  Image is too big; limiting to the first %u sectors\n", kMaxLBASectorCapacity);
        imageSizeInSectors = kMaxLBASectorCapacity;
    }

    // Compute physical parameters according to the size
    m_sectorCapacity = imageSizeInSectors;
    
    // Compute CHS parameters according to hard drive size thresholds
    m_numSectorsPerTrack = 63;

    // Calculate number of heads assuming maximum number of cylinders
    // The thresholds are:
    //        Disk size        | Heads
    // ------------------------+-------
    //     1 B   to 504 MiB    |  16
    //   504 MiB to 1008 MiB   |  32
    //  1008 MiB to 2016 MiB   |  64
    //  2016 MiB to 4032 MiB   |  128
    //  4032 MiB to 8032.5 MiB |  255
    uint32_t numHeads = 16;
    do {
        if (imageSizeInSectors <= 1024 * 63 * numHeads) {
            m_numHeadsPerCylinder = numHeads;
            break;
        }
        numHeads *= 2;
    } while (numHeads < 255);
    if (numHeads > 255) {
        m_numHeadsPerCylinder = 255;
    }
    
    m_numCylinders = imageSizeInSectors / m_numSectorsPerTrack / m_numHeadsPerCylinder;

    return true;
}

ImageHardDriveATADeviceDriver::~ImageHardDriveATADeviceDriver() {
    if (m_fpImage != NULL) {
        fclose(m_fpImage);
    }
}

bool ImageHardDriveATADeviceDriver::ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) {
    // Image not loaded
    if (m_fpImage == NULL) {
        return false;
    }

    // Seek address
    if (fseek(m_fpImage, lbaAddress * kSectorSize, SEEK_SET)) {
        return false;
    }

    // Read data from image
    // TODO: handle copy-on-write
    // If copy-on-write and the sector is copied, read from copy, otherwise read from image file
    // If not copy-on-write, read from image file directly
    int lenRead = fread(destBuffer, 1, kSectorSize, m_fpImage);
    
    // Read is successful if the full sector is read
    return lenRead == kSectorSize;
}

bool ImageHardDriveATADeviceDriver::WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) {
    // Image not loaded
    if (m_fpImage == NULL) {
        return false;
    }

    // Seek address
    if (fseek(m_fpImage, lbaAddress * kSectorSize, SEEK_SET)) {
        return false;
    }

    // Write data to image
    // TODO: handle copy-on-write
    // If copy-on-write and block is copied, overwrite copy, otherwise create copy
    // If not copy-on-write, write to image file directly
    int lenWrite = fwrite(destBuffer, 1, kSectorSize, m_fpImage);
    fflush(m_fpImage);

    return lenWrite == kSectorSize;
}

}
}
}
