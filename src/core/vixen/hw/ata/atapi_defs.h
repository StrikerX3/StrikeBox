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
//   [b] SCSI Block Commands - 3 (SBC-3) Revision 25
//   http://www.13thmonkey.org/documentation/SCSI/sbc3r25.pdf
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

// --- Sizes and capacities ---------------------------------------------------

const uint16_t kDVDSectorSize = 2048;
const uint32_t kMaxSectorsDVDSingleLayer = 2298496;   // 4.7 GiB
const uint32_t kMaxSectorsDVDDualLayer = 4171712;     // 8.5 GiB

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
        OperationCode opCode;                  // byte 0          Operation Code (0x5A)
        uint8_t _reserved1 : 3;                // byte 1 [2:0]    Reserved
        uint8_t disableBlockDescriptors : 1;   // byte 1 [3]      (DBD) Disable Block Descriptors
        uint8_t longLBAAccepted : 1;           // byte 1 [4]      (LLBAA) Long LBA Accepted
        uint8_t _reserved2 : 3;                // byte 1 [7:5]    Reserved
        uint8_t pageCode : 6;                  // byte 2 [5:0]    Page code
        uint8_t pageControl : 2;               // byte 2 [7:6]    (PC) Page control
        uint8_t subpageCode;                   // byte 3          Subpage code
        uint8_t _reserved3[3];                 // byte 4-6        Reserved
        uint8_t length[2];                     // byte 7-8        Allocation Length
        uint8_t control;                       // byte 9          Control
    } modeSense10;

    // [b 5.11 table 56]
    // CDB for the READ (10) command
    struct Read10 {
        OperationCode opCode;                  // byte 0          Operation Code (0x28)
        uint8_t _obsolete1 : 1;                // byte 1 [0]      Obsolete
        uint8_t forceUnitAccessNVCache : 1;    // byte 1 [1]      (FUA_NV) Force unit access non-volatile cache (read from/write to block cache before medium)
        uint8_t _reserved1 : 1;                // byte 1 [2]      Reserved
        uint8_t forceUnitAccess : 1;           // byte 1 [3]      (FUA) Force unit access (force access to media instead of cache)
        uint8_t disablePageOut : 1;            // byte 1 [4]      (DPO) Disable page out (do not cache block)
        uint8_t readProtect : 3;               // byte 1 [7:5]    (RDPROTECT) Read protect
        uint8_t lba[4];                        // byte 2-5        Logical Block Address
        uint8_t groupNumber : 5;               // byte 6 [4:0]    Group number
        uint8_t _reserved2 : 3;                // byte 6 [7:5]    Reserved
        uint8_t length[2];                     // byte 7-8        Transfer length
        uint8_t control;                       // byte 9          Control
    } read10;

    // [m 5.16 table 144]
    // CDB for the READ CAPACITY command
    struct ReadCapacity {
        OperationCode opCode;                  // byte 0          Operation Code (0x25)
        uint8_t _reservedOrObsolete[8];        // byte 1-8        Reserved or obsolete fields
        uint8_t control;                       // byte 9          Control
    } readCapacity;

    // [m 5.20 table 169]
    // CDB for the READ DVD STRUCTURE command
    struct ReadDVDStructure {
        OperationCode opCode;                  // byte 0          Operation Code (0xAD)
        uint8_t _reserved1;                    // byte 1          Reserved
        uint8_t address;                       // byte 2-5        Address
        uint8_t layerNumber;                   // byte 6          Layer number
        uint8_t format;                        // byte 7          Format
        uint8_t length[2];                     // byte 8-9        Allocation Length
        uint8_t _reserved2 : 6;                // byte 10 [5:0]   Reserved
        uint8_t authGrantID : 2;               // byte 10 [7:6]   (AGID) Authenticaton Grant ID (used with formats 2, 6 and 7 when address is 0)
        uint8_t control;                       // byte 11         Control
    } readDVDStructure;

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
    OpModeSense10 = 0x5A,        // (0x5A) MODE SENSE (10 bytes)
    OpRead10 = 0x28,             // (0x28) READ (10 bytes)
    OpReadCapacity = 0x25,       // (0x25) READ CAPACITY
    OpReadDVDStructure = 0xAD,   // (0xAD) READ DVD STRUCTURE
    OpRequestSense = 0x03,       // (0x03) REQUEST SENSE
    OpTestUnitReady = 0x00,      // (0x00) TEST UNIT READY
};

// ----- Operation data -------------------------------------------------------

// [c 10.8.14 table 91] [s 3.22.2 table 120] [m 5.16 table 145]
// Response data for the READ CAPACITY command
struct ReadCapacityData {
    uint8_t lba[4];           // byte 0-3    Logical Block Address
    uint8_t blockLength[4];   // byte 4-7    Block length in bytes (typically 2048)
};

// [m 5.20 table 170]
// Formats for the READ DVD STRUCTURE command
enum DVDStructureFormatType {
    DVDFmtPhysical = 0x00,       // Physical Format Information
    DVDFmtManufacturer = 0x04,   // Disc Manufacturer Information
};

// [m 5.20 subclauses and tables]
// Response data for the READ DVD STRUCTURE command
struct ReadDVDStructureData {
    // Common fields
    uint8_t dataLength[2];              // byte 0-1        Data length
    uint8_t _reserved[2];               // byte 2-3        Reserved

    // [m 5.20.1 tables 171 and 172] Physical Format Information (format 00h)
    union {
        struct PhysicalFormatInformation {
            uint8_t partVersion : 4;         // byte 0 [3:0]     Part version
            uint8_t bookType : 4;            // byte 0 [7:4]     Book type
            uint8_t maxRate : 4;             // byte 1 [3:0]     Maximum rate
            uint8_t discSize : 4;            // byte 1 [7:4]     Disc size
            uint8_t layerType : 4;           // byte 2 [3:0]     Layer type
            uint8_t trackPath : 1;           // byte 2 [4]       Track path
            uint8_t numLayers : 2;           // byte 2 [6:5]     Number of layers
            uint8_t _reserved1 : 1;          // byte 2 [7]       Reserved
            uint8_t trackDensity : 4;        // byte 3 [3:0]     Track density
            uint8_t linearDensity : 4;       // byte 3 [7:4]     Linear density
            uint8_t _zero1;                  // byte 4           00h
            uint8_t dataStartingSector[3];   // byte 5-7         Starting physical sector number of data area
            uint8_t _zero2;                  // byte 8           00h
            uint8_t dataEndingSector[3];     // byte 9-11        Ending physical sector number of data area
            uint8_t _zero3;                  // byte 12          00h
            uint8_t layer0EndingSector[3];   // byte 13-15       Ending physical sector number in layer 0
            uint8_t _reserved2 : 7;          // byte 16 [6:0]    Reserved
            uint8_t burstCuttingArea : 1;    // byte 16 [7]      (BCA) Burst Cutting Area
            // bytes 17 to 2047 are media specific
        } physicalFormatInformation;
    };
};

// [m 5.20.1 table 173]
// Values for the Book Type field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum BookType : uint8_t {
    BookTypeDVDROM = 0b0000,      // DVD-ROM
    BookTypeDVDRAM = 0b0001,      // DVD-RAM
    BookTypeDVDR = 0b0010,        // DVD-R
    BookTypeDVDRW = 0b0011,       // DVD-RW
    BookTypeDVDplusRW = 0b1001,   // DVD+RW
};

// [m 5.20.1]
// Values for the Disc Size field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum DiscSize : uint8_t {
    DiscSize120mm = 0b0000,   // 120 mm disc
    DiscSize80mm = 0b0001,    // 80 mm disc
};

// [m 5.20.1 table 174]
// Values for the Maximum Rate field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum MaximumRate : uint8_t {
    MaxRate2_52Mbps = 0b0000,       // 2.52 Mbps
    MaxRate5_04Mbps = 0b0001,       // 5.04 Mbps
    MaxRate10_08Mbps = 0b0010,      // 10.08 Mbps
    MaxRateNotSpecified = 0b1111,   // Not specified
};

// [m 5.20.1]
// Values for the Number of Layers field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum NumLayers : uint8_t {
    NumLayers1 = 0b00,   // 1 layer
    NumLayers2 = 0b01,   // 2 layers
};

// [m 5.20.1]
// Values for the Track Path field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum TrackPath : uint8_t {
    TrackPathPTP = 0,    // (PTP) Parallel Track Path
    TrackPathOTP = 1,    // (OTP) Opposite Track Path
};

// [m 5.20.1 table 175]
// Values for the Layer Type field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum LayerType : uint8_t {
    LayerTypeEmbossed = (1 << 0),     // Layer contains embossed data
    LayerTypeRecordable = (1 << 1),   // Layer contains recordable data
    LayerTypeRewritable = (1 << 2),   // Layer contains rewritable data
};

// [m 5.20.1 table 176]
// Values for the Linear Density field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum LinearDensity : uint8_t {
    LinearDensity0_267umPerBit = 0b0000,          // 0.267 um/bit
    LinearDensity0_293umPerBit = 0b0001,          // 0.293 um/bit
    LinearDensity0_409To0_435umPerBit = 0b0010,   // 0.409 to 0.435 um/bit
    LinearDensity0_280To0_291umPerBit = 0b0100,   // 0.280 to 0.291 um/bit
    LinearDensity0_353umPerBit = 0b1000,          // 0.353 um/bit
};

// [m 5.20.1 table 177]
// Values for the Track Density field of the READ DVD STRUCTURE format 00h (Physical Format Information) CDB
enum TrackDensity : uint8_t {
    TrackDensity0_74umPerTrack = 0b0000,    // 0.74 um/track
    TrackDensity0_80umPerTrack = 0b0001,    // 0.80 um/track
    TrackDensity0_615umPerTrack = 0b0010,   // 0.615 um/track
};

// [m 5.20.1 table 178]
// Standard starting physical sector number for DVD-ROM, DVD-R/-RW and DVD+RW discs
const uint32_t kStartingSectorNumberDVDROM = 0x30000;

// [m 5.20.1 table 178]
// Standard starting physical sector number for DVD-RAM discs
const uint32_t kStartingSectorNumberDVDRAM = 0x31000;

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

// ----- Operation types ------------------------------------------------------

// Types of packet operation types
enum PacketOperationType {
    PktOpNonData = 0,   // Packet command requires no data
    PktOpDataIn,        // Packet command will read data from the device
    PktOpDataOut,       // Packet command will write data to the device
};

// Maps operation codes to their types
const std::unordered_map<uint8_t, PacketOperationType, std::hash<uint8_t>> kOperationTypes = {
    { OpModeSense10, PktOpDataIn },
    { OpRead10, PktOpDataIn },
    { OpReadCapacity, PktOpDataIn },
    { OpRequestSense, PktOpDataIn },
    { OpReadDVDStructure, PktOpDataIn },
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
