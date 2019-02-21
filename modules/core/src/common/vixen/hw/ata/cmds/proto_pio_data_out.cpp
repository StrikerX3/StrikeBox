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
#include "vixen/hw/ata/cmds/proto_pio_data_out.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

PIODataOutProtocolCommand::PIODataOutProtocolCommand(ATADevice& device)
    : IATACommand(device)
{
}

PIODataOutProtocolCommand::~PIODataOutProtocolCommand() {
}

void PIODataOutProtocolCommand::Execute() {
    bool succeeded = Initialize();
    if (succeeded) {
        m_regs.status |= StDataRequest;
        m_regs.status &= ~StBusy;
    }
    else {
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
        Finish();
    }
}

void PIODataOutProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    // Should never happen
    log_warning("PIODataOutProtocolCommand::ReadData:  Unexpected read!\n");
}

void PIODataOutProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Write to device buffer
    uint32_t lenWritten = WriteBuffer(value, size);
    if (lenWritten != size) {
        log_warning("PIODataOutProtocolCommand::WriteData: Buffer overflow!   channel = %d  device = %d  size = %d  read = %d\n", m_channel, m_devIndex, size, lenWritten);
    }

    // If the transfer finished, process block
    if (m_bufferPos >= kSectorSize) {
        m_regs.status |= StBusy;
        m_regs.status &= ~StDataRequest;

        DrainBuffer();
    }
}

uint32_t PIODataOutProtocolCommand::WriteBuffer(uint8_t *src, uint8_t length) {
    uint32_t lenToWrite = length;
    if (m_bufferPos + length > kSectorSize) {
        lenToWrite = kSectorSize - m_bufferPos;
    }

    memcpy(m_buffer + m_bufferPos, src, lenToWrite);
    m_bufferPos += lenToWrite;
    return lenToWrite;
}

void PIODataOutProtocolCommand::DrainBuffer() {
    auto result = ProcessBlock(m_buffer);

    if (result == BlockWriteError) {
        m_regs.status |= StError;
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
        Finish();
    }
    else if (result == BlockWriteEnd) {
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
        Finish();
    }
    else {
        m_bufferPos = 0;
        m_regs.status |= StDataRequest;
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
    }
}

}
}
}
}
