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
#include "strikebox/hw/ata/cmds/proto_pio_data_in.h"

#include "strikebox/log.h"

namespace strikebox {
namespace hw {
namespace ata {
namespace cmd {

PIODataInProtocolCommand::PIODataInProtocolCommand(ATADevice& device)
    : IATACommand(device)
    , m_bufferPos(0)
{
}

PIODataInProtocolCommand::~PIODataInProtocolCommand() {
}

void PIODataInProtocolCommand::Execute() {
    FillBuffer();
}

void PIODataInProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    // Clear the destination value before reading from the buffer, in case
    // there are not enough bytes to fulfill the request
    *value = 0;
    uint32_t lenRead = ReadBuffer(value, size);
    if (lenRead != size) {
        log_warning("PIODataInProtocolCommand::ReadData:  Buffer underflow!  channel = %d  device = %d  size = %d  read = %d\n", m_channel, m_devIndex, size, lenRead);
    }

    // If the transfer finished, get more data
    if (m_bufferPos >= kSectorSize) {
        if (HasMoreData()) {
            m_regs.status |= StBusy;
            m_regs.status &= ~StDataRequest;
            FillBuffer();
        }
        else {
            m_regs.status &= ~StDataRequest;
            Finish();
        }
    }
}

void PIODataInProtocolCommand::FillBuffer() {
    auto result = ReadBlock(m_buffer);

    if (result == BlockReadError) {
        m_regs.status |= StError;
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
        Finish();
    }
    else if (result == BlockReadEnd) {
        m_regs.status &= ~(StBusy | StDataRequest);
        Finish();
    }
    else {
        m_bufferPos = 0;
        m_regs.status |= StDataRequest;
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
    }
}

uint32_t PIODataInProtocolCommand::ReadBuffer(uint8_t *dst, uint8_t length) {
    uint32_t lenToRead = length;
    if (m_bufferPos + length > kSectorSize) {
        lenToRead = kSectorSize - m_bufferPos;
    }

    memcpy(dst, m_buffer + m_bufferPos, lenToRead);
    m_bufferPos += lenToRead;
    return lenToRead;
}

void PIODataInProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Should never happen
    log_warning("PIODataInProtocolCommand::WriteData:  Unexpected write!\n");
}

}
}
}
}
