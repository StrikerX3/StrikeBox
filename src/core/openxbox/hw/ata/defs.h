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

namespace openxbox {
namespace hw {
namespace ata {

// Resources for the primary IDE channel
// IRQ 14     Command ports 0x1F0 - 0x1F7    Control port 0x3F6
const uint8_t  kPrimaryIRQ              = 14;
const uint16_t kPrimaryCommandBasePort  = 0x1f0;
const uint16_t kPrimaryCommandPortCount = 8;
const uint16_t kPrimaryCommandLastPort  = (kPrimaryCommandBasePort + kPrimaryCommandPortCount - 1);
const uint16_t kPrimaryControlPort      = 0x3f6;

// Resources for the secondary IDE channel
// IRQ 15     Command ports 0x170 - 0x177    Control port 0x376
const uint8_t  kSecondaryIRQ              = 15;
const uint16_t kSecondaryCommandBasePort  = 0x170;
const uint16_t kSecondaryCommandPortCount = 8;
const uint16_t kSecondaryCommandLastPort  = (kSecondaryCommandBasePort + kSecondaryCommandPortCount - 1);
const uint16_t kSecondaryControlPort      = 0x376;


// Command port registers [chapter 7]
enum Register : uint8_t {
    RegData = 0,           // Read/write  [7.7]
    RegError = 1,          // Read-only   [7.11]
    RegFeatures = 1,       // Write-only  [7.12]
    RegSectorCount = 2,    // Read/write  [7.13]
    RegSectorNumber = 3,   // Read/write  [7.14]
    RegCylinderLow = 4,    // Read/write  [7.6]
    RegCylinderHigh = 5,   // Read/write  [7.5]
    RegDeviceHead = 6,     // Read/write  [7.10]
    RegStatus = 7,         // Read-only   [7.15]
    RegCommand = 7,        // Write-only  [7.4]
};

// Control port registers:
//   Alternate Status when reading  [7.3]
//   Device Control when writing    [7.9]

// There also exists a Data port that is used with DMA transfers
// and cannot be directly accessed through port I/O.  [7.8]


// Status bits (read from the Status register)
enum Status : uint8_t {
    StBusy = (1 << 7),          // [7.15.6.1] (BSY) The device is busy
    StReady = (1 << 6),         // [7.15.6.2] (DRDY) The device is ready to accept commands
    StDataRequest = (1 << 3),   // [7.15.6.4] (DRQ) The device is ready to transfer a word of data
    StError = (1 << 0),         // [7.15.6.6] (ERR) An error occurred during execution of the previous command
};

// Device control bits (written to the Device Control register)
enum DeviceControl : uint8_t {
    DevCtlSoftwareReset = (1 << 2),    // [7.9.6] (SRST) Execute a software reset
    DevCtlInterruptEnable = (1 << 1),  // [7.9.6] (nIEN) When set, INTRQ signal is effectively disabled
};

const uint8_t kDeviceHeadSelectorBitMask = (1 << 4);  // [7.10.6] (DEV) Selects Device 0 when cleared or Device 1 when set
const uint8_t kErrorAbortBitMask = (1 << 2);          // [7.11.6] (ABRT) Previous command was aborted due to an error or invalid parameter

enum OperationMode : uint8_t {
    PIO0,
    PIO1,
    PIO2,
    PIO3,
    PIO4,
    UltraDMA0,
    UltraDMA1,
    UltraDMA2,
};

}
}
}
