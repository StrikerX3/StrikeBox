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

#include "proto_pio_data_out.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Implements the Security Unlock command (0xF2) [8.34].
 */
class SecurityUnlock : public PIODataOutProtocolCommand {
public:
    SecurityUnlock(ATADevice& device);
    virtual ~SecurityUnlock() override;

    static IATACommand *Factory(DynamicVariant& sharedMemory, ATADevice& device) { return sharedMemory.Allocate<SecurityUnlock>(device); }

protected:
    bool Initialize() override;
    BlockWriteResult ProcessBlock(uint8_t block[kSectorSize]) override;
};

}
}
}
}
