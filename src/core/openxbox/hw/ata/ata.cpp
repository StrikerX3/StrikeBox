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
#include "ata.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {
namespace hw {
namespace ata {

ATA::ATA(IRQHandler *irqHandler) {
    m_states[0].m_irqHandler = irqHandler;
    m_states[0].m_irqNum = kPrimaryIRQ;
    m_states[0].m_channel = ChanPrimary;

    m_states[1].m_irqHandler = irqHandler;
    m_states[1].m_irqNum = kSecondaryIRQ;
    m_states[1].m_channel = ChanSecondary;
}

ATA::~ATA() {
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
    ATAChannel *state;
    if (port >= kPrimaryCommandBasePort && port <= kPrimaryCommandLastPort) {
        return m_states[ChanPrimary].ReadCommandPort((Register)(port - kPrimaryCommandBasePort), value, size);
    }
    if (port == kPrimaryControlPort) {
        return m_states[ChanPrimary].ReadControlPort(value, size);
    }
    if (port >= kSecondaryCommandBasePort && port <= kSecondaryCommandLastPort) {
        return m_states[ChanSecondary].ReadCommandPort((Register)(port - kPrimaryCommandBasePort), value, size);
    }
    if (port == kSecondaryControlPort) {
        return m_states[ChanSecondary].ReadControlPort(value, size);
    }

    log_warning("ATA::IORead:  Unhandled read!   port = 0x%x,  size = %u\n", port, size);
    *value = 0;
    return false;
}

bool ATA::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    if (port >= kPrimaryCommandBasePort && port <= kPrimaryCommandLastPort) {
        return m_states[ChanPrimary].WriteCommandPort((Register)(port - kPrimaryCommandBasePort), value, size);
    }
    if (port == kPrimaryControlPort) {
        return m_states[ChanPrimary].WriteControlPort(value, size);
    }
    if (port >= kSecondaryCommandBasePort && port <= kSecondaryCommandLastPort) {
        return m_states[ChanSecondary].WriteCommandPort((Register)(port - kSecondaryCommandBasePort), value, size);
    }
    if (port == kSecondaryControlPort) {
        return m_states[ChanSecondary].WriteControlPort(value, size);
    }

    log_warning("ATA::IOWrite: Unhandled write!  port = 0x%x,  size = %u,  value = 0x%x\n", port, size, value);
    return false;
}

}
}
}
