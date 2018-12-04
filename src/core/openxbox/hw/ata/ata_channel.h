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
#include "ata_device.h"
#include "ata_common.h"

namespace openxbox {
namespace hw {
namespace ata {


class ATAChannel {
public:
    ATAChannel(Channel channel, IRQHandler *irqHandler, uint8_t irqNum);
    ~ATAChannel();

    // ----- Basic I/O --------------------------------------------------------

    bool ReadCommandPort(Register reg, uint32_t *value, uint8_t size);
    bool WriteCommandPort(Register reg, uint32_t value, uint8_t size);

    bool ReadControlPort(uint32_t *value, uint8_t size);
    bool WriteControlPort(uint32_t value, uint8_t size);

private:
    friend class ATA;

    Channel m_channel;

    // ----- Devices ----------------------------------------------------------

    ATADevice *m_devs[2];

    // ----- Registers --------------------------------------------------------

    ATARegisters m_regs;

    // ----- State ------------------------------------------------------------

    bool m_interrupt = false;  // [5.2.9] INTRQ (Device Interrupt)

    // ----- Interrupt handling -----------------------------------------------

    void SetInterrupt(bool asserted);

    IRQHandler *m_irqHandler;
    uint8_t m_irqNum;

    // ----- Command port operations ------------------------------------------

    void ReadData(uint16_t *value);
    void ReadStatus(uint8_t *value);

    void WriteData(uint16_t value);
    void WriteCommand(uint8_t value);

    // ----- Utility functions ------------------------------------------------
    
    // Retrieves the index of the currently selected device from bit 4
    // (DEV - Device select) of the Device/Head register [7.10.6]
    inline uint8_t GetSelectedDeviceIndex() const { return (m_regs.deviceHead >> kDevSelectorBit) & 1; }

    inline bool AreInterruptsEnabled() const { return (m_regs.control & DevCtlNegateInterruptEnable) == 0; }
};

}
}
}
