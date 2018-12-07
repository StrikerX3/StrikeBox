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

#include "ata_command.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Base class for all commands based on the DMA protocol [9.10].
 */
class DMAProtocolCommand : public IATACommand {
public:
    DMAProtocolCommand(ATADevice& device, bool isWrite);
    virtual ~DMAProtocolCommand() override;

    // ----- Low-level operations ---------------------------------------------

    void Execute() override;
    void ReadData(uint8_t *value, uint32_t size) override;
    void WriteData(uint8_t *value, uint32_t size) override;

protected:
    // ----- Protocol operations ----------------------------------------------

    // Invoked when the DMA transfer finishes. Updates registers and marks the
    // command as finished.
    void FinishTransfer();

    // Invoked when an unrecoverable error occurred. Updates registers and
    // marks the command as finished.
    void UnrecoverableError();

    // ----- Parameters -------------------------------------------------------

    // Range of operation
    uint64_t m_startingByte;  // Inclusive
    uint64_t m_endingByte;    // Exclusive

    uint64_t m_currentByte;

    // DMA operation type (true = write, false = read), used for sanity check.
    // Specified in the constructor.
    bool m_isWrite;
};

}
}
}
}
