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
#include "strikebox/hw/ata/drvs/drv_vhd_dummy.h"

#include "strikebox/log.h"
#include "strikebox/io.h"

namespace strikebox {
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
    strcpy(m_serialNumber, "0123456789");
    strcpy(m_firmwareRevision, "1.0.0");
    strcpy(m_modelNumber, "SBx DHDD0010000");

    // Initialize with parameters for a 10 GiB hard drive
    m_numCylinders = 20480;
    m_numHeadsPerCylinder = 16;
    m_numSectorsPerTrack = 63;
    m_sectorCapacity = m_numCylinders * m_numHeadsPerCylinder * m_numSectorsPerTrack;
}

DummyHardDriveATADeviceDriver::~DummyHardDriveATADeviceDriver() {
}

bool DummyHardDriveATADeviceDriver::Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) {
    // Fill with zeros, as if the disk was blank
    memset(buffer, 0, size);

    // Always succeed
    return true;
}

bool DummyHardDriveATADeviceDriver::Write(uint64_t byteAddress, uint8_t *buffer, uint32_t size) {
    // Lie about writing, always succeed
    return true;
}

}
}
}
