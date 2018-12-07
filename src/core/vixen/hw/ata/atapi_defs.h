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

namespace vixen {
namespace hw {
namespace atapi {

// --- Command Descriptor Block (CDB) [s 2.1] -----------------------------------

// [s 2.2.1] Operation Code
union OperationCode {
    struct {
        uint8_t commandCode : 5;
        uint8_t groupCode : 3;
    } fields;
    uint8_t u8;
};

// [s 2.2.1 table 9] Values for the Group Code field of the Operation Code
enum GroupCode {
    Grp6Byte = 0b000,      // 6 byte commands
    Grp10Byte1 = 0b001,    // 10 byte commands
    Grp10Byte2 = 0b010,    // 10 byte commands
    Grp16Byte = 0b100,     // 16 byte commands
    Grp12Byte = 0b101,     // 12 byte commands
    GrpVendor1 = 0b110,    // Vendor specific
    GrpVendor2 = 0b111,    // Vendor specific
};

// --- Fixed length CDBs --------------------------------------------------------

// [s 2.1.2 table 4] 12 byte Command Descriptor Block
struct CommandDescriptorBlock12Byte {
    OperationCode opCode;   // byte 0      Operation Code
    uint8_t svcAction : 5;  // byte 1      Service Action
    uint8_t misc1 : 3;      //             Miscellaneous CDB information
    uint32_t lba;           // byte 2-5    Logical Block Address
    uint32_t length;        // byte 6-9    Transfer length, parameter list length or allocation length
    uint8_t misc2;          // byte 10     Miscellaneous CDB information
    uint8_t control;        // byte 11     Control
};

// [s 2.1.2 table 5] 16 byte Command Descriptor Block
struct CommandDescriptorBlock16Byte {
    OperationCode opCode;   // byte 0      Operation Code
    uint8_t misc1;          // byte 1      Miscellaneous CDB information
    uint64_t lba;           // byte 2-9    Logical Block Address
    uint32_t length;        // byte 10-13  Transfer length, parameter list length or allocation length
    uint8_t misc2;          // byte 14     Miscellaneous CDB information
    uint8_t control;        // byte 15     Control
};

}
}
}
