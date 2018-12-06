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

#include "vixen/cpu.h"
#include "../basic/irq.h"
#include "ata_device.h"
#include "ata_common.h"

namespace vixen {
namespace hw {
namespace ata {

/*!
 * Represents one of the two ATA channels in a machine (primary or secondary).
 *
 * An ATA channel contains two devices, typically called master and slave.
 */
class ATAChannel {
public:
    ATAChannel(Channel channel, IRQHandler& irqHandler, uint8_t irqNum);
    ~ATAChannel();

    ATADevice& GetDevice(uint8_t deviceIndex) { return *m_devs[deviceIndex]; }

    // ----- Basic I/O --------------------------------------------------------

    bool ReadCommandPort(Register reg, uint32_t *value, uint8_t size);
    bool WriteCommandPort(Register reg, uint32_t value, uint8_t size);

    bool ReadControlPort(uint32_t *value, uint8_t size);
    bool WriteControlPort(uint32_t value, uint8_t size);

    // ----- DMA transfers ----------------------------------------------------

    bool ReadDMA(uint8_t dstBuffer[kSectorSize]);
    bool WriteDMA(uint8_t srcBuffer[kSectorSize]);
    bool IsDMAFinished();
    bool EndDMA();

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

    IRQHandler& m_irqHandler;
    uint8_t m_irqNum;

    // ----- Command port operations ------------------------------------------

    void ReadData(uint32_t *value, uint8_t size);
    void ReadStatus(uint8_t *value);

    void WriteData(uint32_t value, uint8_t size);
    void WriteCommand(uint8_t value);
};

}
}
}
