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

#include "../../ata/defs.h"
#include "../ata_common.h"

namespace openxbox {
namespace hw {
namespace ata {

/*!
 * Interface for an ATA device driver.
 * ATA device drivers act as virtual devices attached to ATA channels.
 * They typically interact with real hardware by accessing a hard drive image,
 * an XISO, a host DVD drive or similar methods.
 */
class IATADeviceDriver {
public:
    virtual ~IATADeviceDriver();
    virtual bool IsAttached() = 0;
    virtual void IdentifyDevice(IdentifyDeviceData *data) = 0;
};

}
}
}
