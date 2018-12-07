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
#include "proto_nondata.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

NonDataProtocolCommand::NonDataProtocolCommand(ATADevice& device)
    : IATACommand(device) {
}

NonDataProtocolCommand::~NonDataProtocolCommand() {
}

void NonDataProtocolCommand::Execute() {
    bool successful = ExecuteImpl();
    
    auto& regs = m_device.GetRegisters();
    if (!successful) {
        regs.status |= StError;
    }

    regs.status &= ~StBusy;
    m_interrupt.Assert();

    Finish();
}

void NonDataProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    // Should never happen
    log_warning("NonDataProtocolCommand::ReadData:  Unexpected read!\n");
}

void NonDataProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Should never happen
    log_warning("NonDataProtocolCommand::WriteData:  Unexpected write!\n");
}

}
}
}
}
