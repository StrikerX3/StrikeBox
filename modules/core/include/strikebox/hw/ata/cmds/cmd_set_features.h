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

#include "proto_nondata.h"

namespace strikebox {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Implements the Set Features command (0xEF) [8.37].
 */
class SetFeatures : public NonDataProtocolCommand {
public:
    SetFeatures(ATADevice& device);
    virtual ~SetFeatures() override;

    static IATACommand *Factory(DynamicVariant& sharedMemory, ATADevice& device) { return sharedMemory.Allocate<SetFeatures>(device); }

protected:
    bool ExecuteImpl() override;

private:
    // ----- Subcommands ------------------------------------------------------

    bool SetTransferMode();
    bool SetPIOTransferMode(PIOTransferType type, uint8_t mode);
    bool SetDMATransferMode(DMATransferType type, uint8_t mode);
};

}
}
}
}
