// ATAPI Command set emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement a subset of the ATAPI Command set used by the
// Original Xbox to access the DVD drive.
//
// Based on:
//   [a] ATA Packet Interface for CD-ROMs Revision 2.6 Proposed
//   http://www.bswd.com/sff8020i.pdf
//
//   [s] SCSI Commands Reference Manual 100293068, Rev. J
//   https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
//
//   [p] SCSI Primary Commands - 3 (SPC-3)
//   http://www.13thmonkey.org/documentation/SCSI/spc3r23.pdf
//
//   [m] SCSI-3 - Multimedia Commands
//   http://www.13thmonkey.org/documentation/SCSI/x3_304_1997.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification. References are prefixed by
// the letter in brackets as listed above.
#pragma once

#include <cstdint>

#include "atapi_defs.h"

namespace vixen {
namespace hw {
namespace atapi {

// Types of packet operation types
enum PacketOperationType {
    PktOpNonData = 0,   // Packet command requires no data
    PktOpDataIn,        // Packet command will read data from the device
    PktOpDataOut,       // Packet command will write data to the device
};

// Information about a packet command
struct PacketInformation {
    // The operation code of the packet command
    OperationCode opCode;

    // TODO: parsed command data as an union

    // Indicates if the packet was processed or parsed successfully and is supported.
    bool successful;

    // The type of operation described by the packet
    PacketOperationType operationType;

    // The size of the transfer required by the packet command.
    // Only valid if the operation type is not non-data
    uint32_t transferSize;

    // The sense key for the error encountered while processing the packet
    // command. Filled in by the device driver while transferring data.
    uint8_t senseKey = 0;

    // Whether the command was aborted because of an invalid operation code or
    // parameter, or because the device cannot complete the operation.
    bool aborted = false;

    // Indicates that the data transfer has reached the end of the medium.
    bool endOfMedium = false;

    // Indicates that the transfer length of a data transfer was incorrect.
    bool incorrectLengthIndicator = false;

    // Indicates that the device driver encountered a problem executing the command.
    bool deviceFault = false;
};

}
}
}
