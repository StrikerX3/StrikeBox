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
 * Possible outcomes for the attempt to read a block of data.
 */
enum BlockWriteResult {
    BlockWriteOK,     // Block successfully written
    BlockWriteEnd,    // Last block reached
    BlockWriteError,  // An error occurred during the write operation
};

/*!
 * Base class for all commands based on the PIO data out protocol [9.8].
 */
class PIODataOutProtocolCommand : public IATACommand {
public:
    PIODataOutProtocolCommand(ATADevice& device);
    virtual ~PIODataOutProtocolCommand() override;

    // ----- Low-level operations ---------------------------------------------

    void Execute() override;
    void ReadData(uint8_t *value, uint32_t size) override;
    void WriteData(uint8_t *value, uint32_t size) override;

protected:
    // ----- Protocol operations ----------------------------------------------

    // Initializes the command.
    // Returns false in case of error.
    virtual bool Initialize() = 0;

    // Writes the next block.
    // Returns BlockWriteOK if the next block was written successfully.
    // Returns BlockWriteEnd if there are no more blocks to write.
    // Returns BlockWriteError if there was an error writing the next block.
    virtual BlockWriteResult ProcessBlock(uint8_t block[kSectorSize]) = 0;

private:
    uint8_t m_buffer[kSectorSize];
    uint16_t m_bufferPos = 0;

    uint32_t WriteBuffer(uint8_t *src, uint8_t length);
    void DrainBuffer();
};

}
}
}
}
