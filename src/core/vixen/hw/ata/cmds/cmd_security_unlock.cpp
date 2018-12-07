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
#include "cmd_security_unlock.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

SecurityUnlock::SecurityUnlock(ATADevice& device)
    : PIODataOutProtocolCommand(device) {
}

SecurityUnlock::~SecurityUnlock() {
}

bool SecurityUnlock::Initialize() {
    // [8.34.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    return true;
}

BlockWriteResult SecurityUnlock::ProcessBlock(uint8_t block[kSectorSize]) {
    m_done = true;

    if (!m_driver->SecurityUnlock(block)) {
        return BlockWriteError;
    }

    return BlockWriteOK;
}

}
}
}
}
