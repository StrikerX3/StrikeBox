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

#include "openxbox/cpu.h"
#include "../basic/irq.h"
#include "../ata/defs.h"

namespace openxbox {
namespace hw {
namespace ata {

enum Channel {
    ChanPrimary,
    ChanSecondary,
};


struct ATAChannel {
    Channel m_channel;

    // ----- IRQ handling -----------------------------------------------------

    IRQHandler *m_irqHandler;
    uint8_t m_irqNum;

    // ----- Registers --------------------------------------------------------

    OperationMode m_operationMode = PIO0;
    uint8_t m_reg_error = 0;
    uint8_t m_reg_features = 0;
    uint8_t m_reg_sectorCount = 0;
    uint8_t m_reg_sectorNumber = 0;
    uint16_t m_reg_cylinder = 0;
    uint8_t m_reg_deviceHead = 0;

    // ----- State ------------------------------------------------------------

    bool m_interrupt = false;  // [5.2.9] INTRQ (Device Interrupt)

    // ----- Basic I/O --------------------------------------------------------

    bool ReadCommandPort(Register reg, uint32_t *value, uint8_t size);
    bool WriteCommandPort(Register reg, uint32_t value, uint8_t size);

    bool ReadControlPort(uint32_t *value, uint8_t size);
    bool WriteControlPort(uint32_t value, uint8_t size);

    // ----- Command operations -----------------------------------------------

    void ReadData(uint16_t *value);
    void ReadStatus(uint8_t *value);

    void WriteData(uint16_t value);
    void WriteCommand(uint8_t value);

    // ----- Interrupt handling -----------------------------------------------

    void HandleInterrupt(bool asserted);

    // ----- Utility functions ------------------------------------------------

    // Retrieves the index of the currently selected device from bit 4
    // (DEV - Device select) of the Device/Head register [7.10.6]
    inline uint8_t GetSelectedDeviceIndex() const { return (m_reg_deviceHead >> kDeviceHeadSelectorBit) & 1; }

    // Determines if the previous command was aborted due to an error by
    // reading bit 2 (ABRT - Command aborted) from the Error register [7.11.6]
    inline bool IsAborted() const { return (m_reg_error >> kErrorAbortBit) & 1; }

    inline bool IsPIOMode() const { return m_operationMode >= PIO0 && m_operationMode <= PIO4; }
    inline bool IsUltraDMAMode() const { return m_operationMode >= UltraDMA0 && m_operationMode <= UltraDMA2; }

    inline void SetIRQ(bool level) const { m_irqHandler->HandleIRQ(m_irqNum, level); }
};

}
}
}
