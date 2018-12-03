#pragma once

#include <cstdint>

#include "openxbox/cpu.h"
#include "../basic/i8259.h"

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

// IDE channels
enum Channel : uint8_t {
    ChanPrimary = 0,
    ChanSecondary = 1,
};


// Status bits (read from the Status register)
enum Status : uint8_t {
    StBusy = (1 << 7),          // [7.15.6.1] (BSY) The device is busy
    StReady = (1 << 6),         // [7.15.6.2] (DRDY) The device is ready to accept commands
    StDataRequest = (1 << 3),   // [7.15.6.4] (DRQ) The device is ready to transfer a word of data
    StError = (1 << 0),         // [7.15.6.6] (ERR) An error occurred during execution of the previous command
};

// Device control bits (written to the Device Control register)
enum DeviceControl : uint8_t {
    DevCtlSoftwareReset = (1 << 2),    // (SRST) Execute a software reset
    DevCtlInterruptEnable = (1 << 1),  // (nIEN) When set, INTRQ signal is effectively disabled
};

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


class ATA : public IODevice {
public:
    ATA(i8259 *pic);
    virtual ~ATA();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;

private:
    i8259 *m_pic;
  
    bool ReadCommandPort(Register reg, Channel channel, uint32_t *value, uint8_t size);
    bool WriteCommandPort(Register reg, Channel channel, uint32_t value, uint8_t size);

    bool ReadControlPort(Channel channel, uint32_t *value, uint8_t size);
    bool WriteControlPort(Channel channel, uint32_t value, uint8_t size);

    // Command operations
    bool ReadData(Channel channel, uint32_t *value, uint8_t size);
    bool ReadError(Channel channel, uint32_t *value, uint8_t size);
    bool ReadStatus(Channel channel, uint32_t *value, uint8_t size);

    bool WriteData(Channel channel, uint32_t value, uint8_t size);
    bool WriteCommand(Channel channel, uint32_t value, uint8_t size);

    // Utility functions
    static inline uint8_t GetIRQNum(Channel channel) {
        if (channel == ChanPrimary) {
            return kPrimaryIRQ;
        }
        return kSecondaryIRQ;
    }

    inline void SetIRQ(Channel channel, bool level) const { m_pic->HandleIRQ(GetIRQNum(channel), level); }

    // Register utility functions
    inline uint8_t GetSelectedDeviceIndex() const { return (m_reg_deviceHead >> 4) & 1; }

    inline bool IsPIOMode() const { return m_operationMode >= PIO0 && m_operationMode <= PIO4; }
    inline bool IsUltraDMAMode() const { return m_operationMode >= UltraDMA0 && m_operationMode <= UltraDMA2; }

    // Registers
    OperationMode m_operationMode = PIO0;
    uint8_t m_reg_features = 0;
    uint8_t m_reg_sectorCount = 0;
    uint8_t m_reg_sectorNumber = 0;
    uint16_t m_reg_cylinder = 0;
    uint8_t m_reg_deviceHead = 0;
};

}
}
}
