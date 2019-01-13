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

#include "proto_packet.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Implements the Packet command (0xA0) [8.45].
 */
class Packet : public PacketProtocolCommand {
public:
    Packet(ATADevice& device);
    virtual ~Packet() override;

    static IATACommand *Factory(SharedMemory& sharedMemory, ATADevice& device) { return sharedMemory.Allocate<Packet, ATADevice&>(device); }
};

}
}
}
}
