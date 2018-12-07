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
 * Base class for all commands based on the non-data protocol [9.9].
 */
class NonDataProtocolCommand : public IATACommand {
public:
    NonDataProtocolCommand(ATADevice& device);
    virtual ~NonDataProtocolCommand() override;

    // ----- Low-level operations ---------------------------------------------

    void Execute() override;
    void ReadData(uint32_t *value, uint32_t size) override;
    void WriteData(uint32_t value, uint32_t size) override;

protected:
    // ----- Protocol operations ----------------------------------------------

    // Executes the command.
    // Return value indicates success (true) or failure (false).
    virtual bool ExecuteImpl() = 0;
};

}
}
}
}
