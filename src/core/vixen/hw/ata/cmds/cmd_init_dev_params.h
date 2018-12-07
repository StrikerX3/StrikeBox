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

#include "proto_nondata.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Implements the Initialize Device Parameters command (0x91) [8.16].
 */
class InitializeDeviceParameters : public NonDataProtocolCommand {
public:
    InitializeDeviceParameters(ATADevice& device);
    virtual ~InitializeDeviceParameters() override;

    static IATACommand *Factory(ATADevice& device) { return new InitializeDeviceParameters(device); }

protected:
    bool ExecuteImpl() override;
};

}
}
}
}
