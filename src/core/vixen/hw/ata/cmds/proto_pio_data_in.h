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
enum BlockReadResult {
    BlockReadOK,     // Block successfully read
    BlockReadEnd,    // Last block reached
    BlockReadError,  // An error occurred during the read operation
};

/*!
 * Base class for all commands based on the PIO data in protocol [9.7].
 */
class PIODataInProtocolCommand : public IATACommand {
public:
    PIODataInProtocolCommand(ATADevice& device);
    virtual ~PIODataInProtocolCommand() override;

    // ----- Low-level operations ---------------------------------------------

    void Execute() override;
    void ReadData(uint32_t *value, uint32_t size) override;
    void WriteData(uint32_t value, uint32_t size) override;

protected:
    // ----- Protocol operations ----------------------------------------------

    // Reads the next block.
    // Returns BlockReadOK if the next block is available and was read without error.
    // Returns BlockReadEnd if there are no more blocks to read.
    // Returns BlockReadError if there was an error reading the next block.
    virtual BlockReadResult ReadBlock(uint8_t block[kSectorSize]) = 0;

    // Determines if there are more blocks to read from the device.
    virtual bool HasMoreData() = 0;

private:
    uint8_t m_buffer[kSectorSize];
    uint16_t m_bufferPos;

    void FillBuffer();
    uint32_t ReadBuffer(uint8_t *dst, uint8_t size);
};

}
}
}
}
