#pragma once

#include <cstdint>

#include "openxbox/cpu.h"
#include "../basic/i8259.h"


// ATA/ATAPI-4 specification:
// http://www.t13.org/documents/UploadedDocuments/project/d1153r18-ATA-ATAPI-4.pdf

namespace openxbox {
namespace hw {
namespace ata {

#define PRIMARY_ATA_IRQ                   14
#define PORT_PRIMARY_ATA_COMMAND_BASE     0x1f0
#define PORT_PRIMARY_ATA_COMMAND_COUNT    8
#define PORT_PRIMARY_ATA_COMMAND_LAST     (PORT_PRIMARY_ATA_COMMAND_BASE + PORT_PRIMARY_ATA_COMMAND_COUNT - 1)
#define PORT_PRIMARY_ATA_CONTROL          0x3f6

#define SECONDARY_ATA_IRQ                 15
#define PORT_SECONDARY_ATA_COMMAND_BASE   0x170
#define PORT_SECONDARY_ATA_COMMAND_COUNT  8
#define PORT_SECONDARY_ATA_COMMAND_LAST   (PORT_SECONDARY_ATA_COMMAND_BASE + PORT_SECONDARY_ATA_COMMAND_COUNT - 1)
#define PORT_SECONDARY_ATA_CONTROL        0x376

// Command port registers
enum Register : uint8_t {
    RegData = 0,           // Read/write
    RegError = 1,          // Read-only
    RegFeatures = 1,       // Write-only
    RegSectorCount = 2,    // Read/write
    RegSectorNumber = 3,   // Read/write
    RegCylinderLow = 4,    // Read/write
    RegCylinderHigh = 5,   // Read/write
    RegDeviceHead = 6,     // Read/write
    RegStatus = 7,         // Read-only
    RegCommand = 7,        // Write-only
};

enum Channel : uint8_t {
    ChanPrimary = 0,
    ChanSecondary = 1,
};

// Control port has two registers:
//   Alternate Status when reading
//   Device Control when writing

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

    // Registers
    uint8_t m_reg_features;
    uint8_t m_reg_sectorCount;
    uint8_t m_reg_sectorNumber;
    uint16_t m_reg_cylinder;
    uint8_t m_reg_deviceHead;
};

}
}
}
