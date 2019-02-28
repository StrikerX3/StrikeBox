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

#include "proto_dma.h"

namespace strikebox {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Implements the Write DMA command (0xCA) [8.45].
 */
class WriteDMA : public DMAProtocolCommand {
public:
    WriteDMA(ATADevice& device);
    virtual ~WriteDMA() override;

    static IATACommand *Factory(DynamicVariant& sharedMemory, ATADevice& device) { return sharedMemory.Allocate<WriteDMA>(device); }
};

}
}
}
}
