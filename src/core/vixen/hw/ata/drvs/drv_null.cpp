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
#include "drv_null.h"

#include "vixen/log.h"
#include "vixen/io.h"

namespace vixen {
namespace hw {
namespace ata {

NullATADeviceDriver g_nullATADeviceDriver;

NullATADeviceDriver::~NullATADeviceDriver() {
}

void NullATADeviceDriver::IdentifyDevice(IdentifyDeviceData *data) {
    // Fill in with zeros
    memset(data, 0, sizeof(IdentifyDeviceData));
}

}
}
}
