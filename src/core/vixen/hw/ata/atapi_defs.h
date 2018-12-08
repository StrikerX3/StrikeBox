// ATAPI Command set emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement a subset of the ATAPI Command set used by the
// Original Xbox to access the DVD drive.
//
// Based on:
//   [p] SCSI Primary Commands - 3 (SPC-3) Draft
//   http://t10.org/ftp/t10/document.08/08-309r1.pdf
//
//   [m] SCSI Multimedia Commands - 3 (MMC-3) Revision 10g
//   https://www.rockbox.org/wiki/pub/Main/DataSheets/mmc2r11a.pdf
//
//   [a] SCSI Architecture Model - 3 (SAM-3) Revision 13
//   http://www.csit-sun.pub.ro/~cpop/Documentatie_SMP/Standarde_magistrale/SCSI/sam3r13.pdf
//
//   [c] ATA Packet Interface for CD-ROMs Revision 2.6 Proposed
//   http://www.bswd.com/sff8020i.pdf
//
//   [s] SCSI Commands Reference Manual 100293068, Rev. J
//   https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification. References are prefixed by
// the letter in brackets as listed above.
#pragma once

#include <cstdint>

namespace vixen {
namespace hw {
namespace atapi {

// --- Command Descriptor Block (CDB) [p 4.3] ---------------------------------

// [p 4.3.4.1] Operation Code
union OperationCode {
    struct {
        uint8_t commandCode : 5;
        uint8_t groupCode : 3;
    } fields;
    uint8_t u8;
};

// [p 4.3.4.1 table 11] Values for the Group Code field of the Operation Code
enum GroupCode {
    Grp6Byte = 0b000,      // 6 byte commands
    Grp10Byte1 = 0b001,    // 10 byte commands
    Grp10Byte2 = 0b010,    // 10 byte commands
    Grp16Byte = 0b100,     // 16 byte commands
    Grp12Byte = 0b101,     // 12 byte commands
    GrpVendor1 = 0b110,    // Vendor specific
    GrpVendor2 = 0b111,    // Vendor specific
};

// All Command Descriptor Block types in an union, for easy manipulation.
// Note that all multi-byte fields are in big endian order.
union CommandDescriptorBlock {
    // [p 4.3.4.1] The operation code (the first byte in every CDB)
    OperationCode opCode;

    // [p 4.3.2 table 3] [s 2.1.2 table 2]
    // Generic 6 byte Command Descriptor Block
    struct Generic6Byte {
        OperationCode opCode;   // byte 0          Operation Code
        uint8_t lba3 : 5;       // byte 1 [4:0]    Logical Block Address [21:16]
        uint8_t misc1 : 3;      // byte 1 [7:5]    Miscellaneous CDB information
        uint8_t lba2;           // byte 2          Logical Block Address [15:8]
        uint8_t lba1;           // byte 3          Logical Block Address [7:0]
        uint8_t length;         // byte 4          Transfer length, parameter list length or allocation length
        uint8_t control;        // byte 5          Control
    } generic6Byte;

    // [p 4.3.2 table 4] [s 2.1.2 table 3]
    // Generic 10 byte Command Descriptor Block
    struct Generic10Byte {
        OperationCode opCode;   // byte 0          Operation Code
        uint8_t svcAction : 5;  // byte 1 [4:0]    Service Action
        uint8_t misc1 : 3;      // byte 1 [7:5]    Miscellaneous CDB information
        uint8_t lba[4];         // byte 2-5        Logical Block Address
        uint8_t misc2;          // byte 6          Miscellaneous CDB information
        uint8_t length[2];      // byte 7-8        Transfer length, parameter list length or allocation length
        uint8_t control;        // byte 9          Control
    } generic10Byte;

    // [p 4.3.2 table 5] [s 2.1.2 table 4]
    // Generic 12 byte Command Descriptor Block
    struct Generic12Byte {
        OperationCode opCode;   // byte 0          Operation Code
        uint8_t svcAction : 5;  // byte 1 [4:0]    Service Action
        uint8_t misc1 : 3;      // byte 1 [7:5]    Miscellaneous CDB information
        uint8_t lba[4];         // byte 2-5        Logical Block Address
        uint8_t length[4];      // byte 6-9        Transfer length, parameter list length or allocation length
        uint8_t misc2;          // byte 10         Miscellaneous CDB information
        uint8_t control;        // byte 11         Control
    } generic12Byte;

    // [p 4.3.2 table 6] [s 2.1.2 table 5]
    // Generic 16 byte Command Descriptor Block
    struct Generic16Byte {
        OperationCode opCode;   // byte 0          Operation Code
        uint8_t misc1;          // byte 1          Miscellaneous CDB information
        uint8_t lba[8];         // byte 2-9        Logical Block Address
        uint8_t length[4];      // byte 10-13      Transfer length, parameter list length or allocation length
        uint8_t misc2;          // byte 14         Miscellaneous CDB information
        uint8_t control;        // byte 15         Control
    } generic16Byte;

    // [p 6.10 table 99] [s 3.12 table 75]
    // CDB for the MODE SENSE (10) command
    struct ModeSense10 {
        OperationCode opCode;                  // byte 0        Operation Code (0x5A)
        uint8_t _reserved1 : 3;                // byte 1 [2:0]  Reserved
        uint8_t disableBlockDescriptors : 1;   // byte 1 [3]    (DBD) Disable Block Descriptors
        uint8_t longLBAAccepted : 1;           // byte 1 [4]    (LLBAA) Long LBA Accepted
        uint8_t _reserved2 : 3;                // byte 1 [7:5]  Reserved
        uint8_t pageCode : 6;                  // byte 2 [5:0]  Page code
        uint8_t pageControl : 2;               // byte 2 [7:6]  (PC) Page control
        uint8_t subpageCode;                   // byte 3        Subpage code
        uint8_t _reserved3[3];                 // byte 4-6      Reserved
        uint8_t length[2];                     // byte 7-8      Allocation Length
        uint8_t control;                       // byte 9        Control
    } modeSense10;

    // [m 5.16 table 144]
    // CDB for the READ CAPACITY command
    struct ReadCapacity {
        OperationCode opCode;                  // byte 0        Operation Code (0x25)
        uint8_t _reservedOrObsolete[8];        // byte 1-8      Reserved or obsolete fields
        uint8_t control;                       // byte 9        Control
    } readCapacity;

    // [p 6.27 table 169] [s 3.37 table 164]
    // CDB for the REQUEST SENSE command
    struct RequestSense {
        OperationCode opCode;   // byte 0      Operation Code (0x03)
        uint8_t _reserved1[3];  // byte 1-3    Reserved
        uint8_t length;         // byte 4      Allocation Length
        uint8_t control;        // byte 5      Control
    } requestSense;

    // [p 6.33 table 183] [s 3.53 table 202]
    // CDB for the TEST UNIT READY command
    struct TestUnitReady {
        OperationCode opCode;                  // byte 0        Operation Code (0x00)
        uint8_t _reserved[4];                  // byte 1-4      Reserved
        uint8_t control;                       // byte 5        Control
    } testUnitReady;
};

// [p 6.9.1 table 97] [s 3.11.1 table 74]
// Values for the Page Control field of the MODE SENSE (6) and (10) CDBs
enum PageControl : uint8_t {
    PCCurrentValues = 0b00,      // Current values
    PCChangeableValues = 0b01,   // Changeable values
    PCDefaultValues = 0b10,      // Default values
    PCSavedValues = 0b11,        // Saved values
};

// ----- Operations -----------------------------------------------------------

// Operation Codes
enum Operations : uint8_t {
    OpModeSense10 = 0x5A,     // MODE SENSE (10 bytes)
    OpReadCapacity = 0x25,    // READ CAPACITY
    OpRequestSense = 0x03,    // REQUEST SENSE
    OpTestUnitReady = 0x00,   // TEST UNIT READY
};

// ----- Operation data -------------------------------------------------------

// [c 10.8.14 table 91] [s 3.22.2 table 120] [m 5.16 table 145]
// Response data for the READ CAPACITY command
struct ReadCapacityData {
    uint8_t lba[4];           // byte 0-3    Logical Block Address
    uint8_t blockLength[4];   // byte 4-7    Block length in bytes (typically 2048)
};

// [c 10.8.20 table 137] [s 2.4.1.2 table 27]
// Response data for the REQUEST SENSE command
struct RequestSenseData {
    uint8_t responseCode : 7;           // byte 0 [6:0]    Response code (0x70 or 0x71)
    uint8_t valid : 1;                  // byte 0 [7]      1 = Information field is valid
    uint8_t _obsolete;                  // byte 1          Obsolete  ([c 10.8.20 table 137] indicates that this used to be Segment Number)
    uint8_t senseKey : 4;               // byte 2 [3:0]    Sense key
    uint8_t _reserved1 : 1;             // byte 2 [4]      Reserved
    uint8_t incorrectLength : 1;        // byte 2 [5]      (ILI) Incorrect Length Indicator
    uint8_t endOfMedia : 1;             // byte 2 [6]      (EOM) End of Media
    uint8_t fileMark : 1;               // byte 2 [7]      Filemark
    uint8_t information[4];             // byte 3-6        Information
    uint8_t additionalSenseLen;         // byte 7          Additional sense length (n-7)
    uint8_t cmdInformation[4];          // byte 8-11       Command-specific information
    uint8_t addSenseCode;               // byte 12         Additional sense code
    uint8_t addSenseCodeQualifier;      // byte 13         Additional sense code qualifier
    uint8_t fieldReplaceableUnitCode;   // byte 14         Field replaceable unit code
    uint8_t senseKeySpecific[3];        // byte 15-17      Sense key specific [22:0] and valid bit [23]
    uint8_t additionalSenseBytesStart;  // byte 18-n       Additional sense bytes
};

// ----- Operation descriptors ------------------------------------------------

// Types of packet operation types
enum PacketOperationType {
    PktOpNonData = 0,   // Packet command requires no data
    PktOpDataIn,        // Packet command will read data from the device
    PktOpDataOut,       // Packet command will write data to the device
};

// Maps operation codes to their types
const std::unordered_map<uint8_t, PacketOperationType, std::hash<uint8_t>> kOperationTypes = {
    { OpModeSense10, PktOpDataIn },
    { OpReadCapacity, PktOpDataIn },
    { OpRequestSense, PktOpDataIn },
    { OpTestUnitReady, PktOpNonData },
};

// ----- Status ---------------------------------------------------------------

// [a 5.3.1 table 21]
// Status codes
enum StatusCode : uint8_t {
    StGood = 0x00,                       // GOOD: Indicates that the device server has successfully completed the task
    StCheckCondition = 0x02,             // CHECK CONDITION: Indicates that sense data has been delivered and additional actions are required
    StConditionMet = 0x04,               // CONDITION MET: Returned whenever the requested operation specified by an unlinked command is satisfied
    StBusy = 0x08,                       // BUSY: Indicates that the logical unit is busy
    StIntermediate = 0x10,               // INTERMEDIATE: Returned for each successfully completed command in a series of linked commands
    StIntermediateConditionMet = 0x14,   // INTERMEDIATE-CONDITION MET: Returned whenever the requested operation specified by a linked command is satisfied
    StReservationConflict = 0x18,        // RESERVATION CONFLICT: Returned whenever a SCSI initiator port attempts to access a logical unit or an element of a logical unit in a way that conflicts with an existing reservation
    StTaskSetFull = 0x28,                // TASK SET FULL: Returned when the logical unit has at least one task in the task set for a SCSI initiator port and a lack of task set resources prevents accepting a received task from that SCSI initiator port into the task set
    StACAActive = 0x30,                  // ACA ACTIVE: Returned when an ACA exists within a task set and a SCSI initiator port issues a command for that task set under specific conditions 
    StTaskAborted = 0x40,                // TASK ABORTED: Returned when a task is aborted by another SCSI initiator port and the Control mode page TAS bit is set to one
};

// ----- Sense ----------------------------------------------------------------

// [p 4.5.6 table 27] [s 2.4.1.5 table 28]
// Sense key codes
enum SenseKey : uint8_t {
    SKNoSense = 0x0,          // NO SENSE: Indicates that there is no specific sense key information to be reported
    SKRecoveredError = 0x1,   // RECOVERED ERROR: Indicates that the command completed successfully, with some recovery action performed by the device server
    SKNotReady = 0x2,         // NOT READY: Indicates that the logical unit is not accessible
    SKMediumError = 0x3,      // MEDIUM ERROR: Indicates that the command terminated with a non-recovered error condition that may have been caused by a flaw in the medium or an error in the recorded data
    SKHardwareError = 0x4,    // HARDWARE ERROR: Indicates that the device server detected a non-recoverable hardware failure while performing the command or during a self test
    SKIllegalRequest = 0x5,   // ILLEGAL REQUEST: Indicates an invalid parameter
    SKUnitAttention = 0x6,    // UNIT ATTENTION: Indicates that a unit attention condition has been established (e.g., the removable medium may have been changed, a logical unit reset occurred)
    SKDataProtect = 0x7,      // DATA PROTECT: Indicates that a command that reads or writes the medium was attempted on a block that is protected
    SKBlankCheck = 0x8,       // BLANK CHECK: Indicates that a write-once device or a sequential-access device encountered blank medium or format-defined end-of-data indication while reading or that a write-once device encountered a non-blank medium while writing
    SKVendorSpecific = 0x9,   // VENDOR SPECIFIC: This sense key is available for reporting vendor specific conditions
    SKCopyAborted = 0xA,      // COPY ABORTED: Indicates an EXTENDED COPY command was aborted due to an error condition on the source device, the destination device, or both
    SKAbortedCommand = 0xB,   // ABORTED COMMAND: Indicates that the device server aborted the command
    // 0xC is reserved
    SKVolumeOverflow = 0xD,   // VOLUME OVERFLOW: Indicates that a buffered SCSI device has reached the end-of-partition and data may remain in the buffer that has not been written to the medium
    SKMiscompare = 0xE,       // MISCOMPARE: Indicates that the source data did not match the data read from the medium
    SKCompleted = 0xF,        // COMPLETED: Indicates there is command completed sense data to be reported
};

// [p D.2 table D.1]
// Additional sense codes and qualifiers.
// The additional sense code is in the most significant byte.
// The additional sense code qualifier is in the least significant byte.
enum AdditionalSenseCode : uint16_t {
    ASCNone = 0x0000,                // No additional sense information
    ASCInvalidFieldInCDB = 0x2400,   // Invalid field in CDB
    ASCMediumNotPresent = 0x3A00,    // Medium not present
};

}
}
}
