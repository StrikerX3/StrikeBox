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
#include "strikebox/hw/ata/ata.h"

#include "strikebox/log.h"
#include "strikebox/io.h"

namespace strikebox {
namespace hw {
namespace ata {

ATA::ATA(IRQHandler& irqHandler) {
    m_channels[0] = new ATAChannel(ChanPrimary, irqHandler, kPrimaryIRQ);
    m_channels[1] = new ATAChannel(ChanSecondary, irqHandler, kSecondaryIRQ);
}

ATA::~ATA() {
    for (uint8_t i = 0; i < 2; i++) {
        delete m_channels[i];
    }
}

void ATA::Reset() {
}

bool ATA::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(kPrimaryCommandBasePort, kPrimaryCommandPortCount, this)) return false;
    if (!mapper->MapIODevice(kPrimaryControlPort, 1, this)) return false;

    if (!mapper->MapIODevice(kSecondaryCommandBasePort, kSecondaryCommandPortCount, this)) return false;
    if (!mapper->MapIODevice(kSecondaryControlPort, 1, this)) return false;

    return true;
}

bool ATA::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    if (port >= kPrimaryCommandBasePort && port <= kPrimaryCommandLastPort) {
        return m_channels[ChanPrimary]->ReadCommandPort((Register)(port - kPrimaryCommandBasePort), value, size);
    }
    if (port == kPrimaryControlPort) {
        return m_channels[ChanPrimary]->ReadControlPort(value, size);
    }
    if (port >= kSecondaryCommandBasePort && port <= kSecondaryCommandLastPort) {
        return m_channels[ChanSecondary]->ReadCommandPort((Register)(port - kPrimaryCommandBasePort), value, size);
    }
    if (port == kSecondaryControlPort) {
        return m_channels[ChanSecondary]->ReadControlPort(value, size);
    }

    log_warning("ATA::IORead:  Unhandled read!   port = 0x%x,  size = %u\n", port, size);
    *value = 0;
    return false;
}

bool ATA::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    if (port >= kPrimaryCommandBasePort && port <= kPrimaryCommandLastPort) {
        return m_channels[ChanPrimary]->WriteCommandPort((Register)(port - kPrimaryCommandBasePort), value, size);
    }
    if (port == kPrimaryControlPort) {
        return m_channels[ChanPrimary]->WriteControlPort(value, size);
    }
    if (port >= kSecondaryCommandBasePort && port <= kSecondaryCommandLastPort) {
        return m_channels[ChanSecondary]->WriteCommandPort((Register)(port - kSecondaryCommandBasePort), value, size);
    }
    if (port == kSecondaryControlPort) {
        return m_channels[ChanSecondary]->WriteControlPort(value, size);
    }

    log_warning("ATA::IOWrite: Unhandled write!  port = 0x%x,  size = %u,  value = 0x%x\n", port, size, value);
    return false;
}

}
}
}
