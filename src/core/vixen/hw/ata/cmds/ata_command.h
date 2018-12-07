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

#include "../ata_device.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Interface for ATA commands.
 */
class IATACommand {
public:
    IATACommand(ATADevice& device);
    virtual ~IATACommand();

    virtual void Execute() = 0;
    virtual void ReadData(uint32_t *value, uint32_t size) = 0;
    virtual void WriteData(uint32_t value, uint32_t size) = 0;
    bool IsFinished();

    typedef IATACommand* (*Factory)(ATADevice& device);

protected:
    ATADevice& m_device;
    ATARegisters& m_regs;
    IATADeviceDriver *m_driver;
    const Channel m_channel;
    const uint8_t m_devIndex;
    InterruptTrigger& m_interrupt;
    bool m_done;
};

}
}
}
}
