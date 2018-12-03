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

#include "openxbox/cpu.h"
#include "../basic/i8259.h"
#include "../ata/defs.h"
#include "ata_channel.h"

namespace openxbox {
namespace hw {
namespace ata {

class ATA : public IODevice {
public:
    ATA(i8259 *pic);
    virtual ~ATA();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;

private:
    ATAChannel m_states[2];
};

}
}
}
