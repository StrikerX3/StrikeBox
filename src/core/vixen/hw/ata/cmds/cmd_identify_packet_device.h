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

#include "proto_pio_data_in.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Implements the Identify Packet Device command (0xA1) [8.13].
 */
class IdentifyPacketDevice : public PIODataInProtocolCommand {
public:
    IdentifyPacketDevice(ATADevice& device);
    virtual ~IdentifyPacketDevice() override;

    static IATACommand *Factory(ATADevice& device) { return new IdentifyPacketDevice(device); }

protected:
    bool HasMoreData() override;
    BlockReadResult ReadBlock(uint8_t data[kSectorSize]) override;
};

}
}
}
}
