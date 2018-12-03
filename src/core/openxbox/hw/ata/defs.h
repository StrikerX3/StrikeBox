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
    RegData = 0,           // [7.7]  Read/write 
    RegError = 1,          // [7.11] Read-only  
    RegFeatures = 1,       // [7.12] Write-only 
    RegSectorCount = 2,    // [7.13] Read/write 
    RegSectorNumber = 3,   // [7.14] Read/write 
    RegCylinderLow = 4,    // [7.6]  Read/write 
    RegCylinderHigh = 5,   // [7.5]  Read/write 
    RegDeviceHead = 6,     // [7.10] Read/write 
    RegStatus = 7,         // [7.15] Read-only  
    RegCommand = 7,        // [7.4]  Write-only 
};

// Register sizes
const uint8_t kRegSizes[] = {
    2,   // [7.7]  Data
    1,   // [7.11] Error and [7.12] Features
    1,   // [7.13] Sector count
    1,   // [7.14] Sector number
    1,   // [7.6]  Cylinder low
    1,   // [7.6]  Cylinder high
    1,   // [7.10] Device/Head
    1,   // [7.15] Status and [7.4] Command
};

// Control port registers:
//   Alternate Status when reading  [7.3]
//   Device Control when writing    [7.9]

// There also exists a Data port that is used with DMA transfers
// and cannot be directly accessed through port I/O.  [7.8]


// Status bits (read from the Status register)
enum StatusBits : uint8_t {
    StBusy = (1 << 7),          // [7.15.6.1] (BSY) The device is busy
    StReady = (1 << 6),         // [7.15.6.2] (DRDY) The device is ready to accept commands
    StDataRequest = (1 << 3),   // [7.15.6.4] (DRQ) The device is ready to transfer a word of data
    StError = (1 << 0),         // [7.15.6.6] (ERR) An error occurred during execution of the previous command
};

// Error bits (read from the Error register)
enum ErrorBits : uint8_t {
    ErrAbort = (1 << 2),        // [7.11.6] (ABRT) Previous command was aborted due to an error or invalid parameter
};


// Device control bits (written to the Device Control register)
enum DeviceControlBits : uint8_t {
    DevCtlSoftwareReset = (1 << 2),          // [7.9.6] (SRST) Execute a software reset
    DevCtlNegateInterruptEnable = (1 << 1),  // [7.9.6] (nIEN) When set, INTRQ signal is effectively disabled
};

const uint8_t kDevSelectorBit = 4;  // [7.10.6] (DEV) Selects Device 0 when cleared or Device 1 when set

// [8.37.10 table 20] Transfer modes for the Set Transfer Mode subcommand of the Set Features command.
// These specify the 5 most significant bits of the transfer mode.
// The 3 least significant bits specify additional parameters for the mode. When applicable, they are documented below.
enum TransferMode : uint8_t {
    XferModePIODefault = 0b00000,     // PIO default mode
                                      //   000b = IORDY enabled
                                      //   001b = IORDY disabled
    XferModePIOFlowCtl = 0b00001,     // PIO flow control transfer mode
                                      //   LSB specify mode (from 0 to 4)
    XferModeMultiWordDMA = 0b00100,   // Multiword DMA mode
                                      //   LSB specify mode (from 0 to 2)
    XferModeUltraDMA = 0b01000,       // Ultra DMA mode
                                      //   LSB specify mode (from 0 to 2)
};

// [8] Commands
enum Command : uint8_t {
    CmdSetFeatures = 0xEF,   // [8.37] Set Features
};

// [8.37.8] Set Features subcommands (specified in the Features register)
enum SetFeaturesSubCommand : uint8_t {
    SFCmdSetTransferMode = 0x03,    // [8.37.10] Set Transfer Mode
};

}
}
}
