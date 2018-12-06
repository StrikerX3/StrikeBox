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
#include "drv_vhd_dummy.h"

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

DummyHardDriveATADeviceDriver::DummyHardDriveATADeviceDriver() {
    // Initialize with parameters for a 10 GiB hard drive
    m_numCylinders = 20480;
    m_numHeadsPerCylinder = 16;
    m_numSectorsPerTrack = 63;
    m_sectorCapacity = m_numCylinders * m_numHeadsPerCylinder * m_numSectorsPerTrack;
}

DummyHardDriveATADeviceDriver::~DummyHardDriveATADeviceDriver() {
}

bool DummyHardDriveATADeviceDriver::ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) {
    // Fill with zeros, as if the disk was blank
    memset(destBuffer, 0, kSectorSize);

    // Always succeed
    return true;
}

bool DummyHardDriveATADeviceDriver::WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) {
    // Lie about writing, always succeed
    return true;
}

}
}
}
