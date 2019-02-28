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
#include "strikebox/hw/ata/drvs/drv_vdvd_dummy.h"

#include "strikebox/log.h"
#include "strikebox/io.h"

namespace strikebox {
namespace hw {
namespace ata {

DummyDVDDriveATADeviceDriver::DummyDVDDriveATADeviceDriver() {
    strcpy(m_serialNumber, "0123456789");
    strcpy(m_firmwareRevision, "1.0.0");
    strcpy(m_modelNumber, "vXn DDVDD0010000");
}

DummyDVDDriveATADeviceDriver::~DummyDVDDriveATADeviceDriver() {
}

bool DummyDVDDriveATADeviceDriver::Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) {
    // Always fail; no media in drive
    return false;
}

}
}
}
