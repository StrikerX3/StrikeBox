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

#include "drv_vhd_base.h"

namespace strikebox {
namespace hw {
namespace ata {

/*!
 * The dummy ATA device driver represents a basic hard drive that is filled with zeros.
 */
class DummyHardDriveATADeviceDriver : public BaseHardDriveATADeviceDriver {
public:
    DummyHardDriveATADeviceDriver();
    ~DummyHardDriveATADeviceDriver() override;

    // ----- Data access ------------------------------------------------------
    
    bool Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) override;
    bool Write(uint64_t byteAddress, uint8_t *buffer, uint32_t size) override;
};

}
}
}
