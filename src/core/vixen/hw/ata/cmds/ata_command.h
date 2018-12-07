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
 *
 * Classes that derive this interface provide base implementations for ATA
 * protocols, which handle the basic flow of the command execution in a device.
 *
 * When a command finishes execution (i.e., reaches the end of the fluxogram),
 * it must invoke Finish() to allow resources to be cleaned up.
 */
class IATACommand {
public:
    IATACommand(ATADevice& device);
    virtual ~IATACommand();

    /*!
     * Execute is invoked upon receiving the command on the Command port.
     * The BSY flag in the Status register is already set to 1.
     */
    virtual void Execute() = 0;

    /*!
     * ReadData is invoked when the host reads from the Data register, or the
     * Bus Master IDE controller reads during a DMA transfer.
     */
    virtual void ReadData(uint8_t *value, uint32_t size) = 0;

    /*!
     * WriteData is invoked when the host writes to the Data register, or the
     * Bus Master IDE controller writes during a DMA transfer.
     */
    virtual void WriteData(uint8_t *value, uint32_t size) = 0;

    /*!
     * Determines if the command finished execution. This is checked after
     * invoking Execute, ReadData and WriteData. Use the Finish method to
     * indicate command completion.
     */
    bool IsFinished() { return m_finished; }

    /*!
     * Defines the factory function type used to build a factory table.
     */
    typedef IATACommand* (*Factory)(ATADevice& device);

protected:
    ATADevice& m_device;
    ATARegisters& m_regs;
    IATADeviceDriver *m_driver;
    const Channel m_channel;
    const uint8_t m_devIndex;
    InterruptTrigger& m_interrupt;

    /*!
     * Marks the command execution as completed so that the emulator can
     * free up resources used by this command.
     */
    void Finish() { m_finished = true; }

private:
    bool m_finished;
};

}
}
}
}
