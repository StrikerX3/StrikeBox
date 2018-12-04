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

    // [8.37.10] PIO and DMA modes are separate
    PIOTransferType m_pioTransferType = XferTypePIODefault;
    uint8_t m_pioTransferMode = 0;
    
    DMATransferType m_dmaTransferType = XferTypeMultiWordDMA;
    uint8_t m_dmaTransferMode = 0;

    uint8_t m_reg_status = 0;
    uint8_t m_reg_error = 0;
    uint8_t m_reg_features = 0;
    uint8_t m_reg_sectorCount = 0;
    uint8_t m_reg_sectorNumber = 0;
    uint16_t m_reg_cylinder = 0;
    uint8_t m_reg_deviceHead = 0;
    uint8_t m_reg_control = 0;

    // ----- State ------------------------------------------------------------

    bool m_interrupt = false;  // [5.2.9] INTRQ (Device Interrupt)

    // ----- Basic I/O --------------------------------------------------------

    bool ReadCommandPort(Register reg, uint32_t *value, uint8_t size);
    bool WriteCommandPort(Register reg, uint32_t value, uint8_t size);

    bool ReadControlPort(uint32_t *value, uint8_t size);
    bool WriteControlPort(uint32_t value, uint8_t size);

    // ----- Command port operations ------------------------------------------

    void ReadData(uint16_t *value);
    void ReadStatus(uint8_t *value);

    void WriteData(uint16_t value);
    void WriteCommand(uint8_t value);

    // ----- Command handlers -------------------------------------------------

    // These functions must return false on error
    bool SetFeatures();

    // ----- Set Features subcommand handlers ---------------------------------
    
    // These functions must return false on error
    bool SetTransferMode();
    bool SetPIOTransferMode(PIOTransferType type, uint8_t mode);
    bool SetDMATransferMode(DMATransferType type, uint8_t mode);

    // ----- Interrupt handling -----------------------------------------------

    void SetInterrupt(bool asserted);

    // ----- Utility functions ------------------------------------------------

    // Retrieves the index of the currently selected device from bit 4
    // (DEV - Device select) of the Device/Head register [7.10.6]
    inline uint8_t GetSelectedDeviceIndex() const { return (m_reg_deviceHead >> kDevSelectorBit) & 1; }

    inline bool AreInterruptsEnabled() const { return (m_reg_control & DevCtlNegateInterruptEnable) == 0; }
};

}
}
}
