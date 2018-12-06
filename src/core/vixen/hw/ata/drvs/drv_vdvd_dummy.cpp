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
#include "drv_vdvd_dummy.h"

#include "vixen/log.h"
#include "vixen/io.h"

namespace vixen {
namespace hw {
namespace ata {

DummyDVDDriveATADeviceDriver::DummyDVDDriveATADeviceDriver() {
    strcpy(m_serialNumber, "1234567890");
    strcpy(m_firmwareRevision, "1.00");
    strcpy(m_modelNumber, "DUMMY DVD 12345");
}

DummyDVDDriveATADeviceDriver::~DummyDVDDriveATADeviceDriver() {
}

bool DummyDVDDriveATADeviceDriver::ReadSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) {
    // Fill with zeros, as if the disk was blank
    memset(destBuffer, 0, kSectorSize);

    // Always succeed
    return true;
}

bool DummyDVDDriveATADeviceDriver::WriteSector(uint32_t lbaAddress, uint8_t destBuffer[kSectorSize]) {
    // Lie about writing, always succeed
    return true;
}

}
}
}
