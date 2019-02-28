// ATAPI Command set emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement the subset of the ATAPI Command set used by the
// Original Xbox to access the DVD drive.
//
// Based on:
//   [p] SCSI Primary Commands - 3 (SPC-3) Draft
//   http://t10.org/ftp/t10/document.08/08-309r1.pdf
//
//   [m] SCSI Multimedia Commands - 3 (MMC-3) Revision 10g
//   https://www.rockbox.org/wiki/pub/Main/DataSheets/mmc2r11a.pdf
//
//   [b] SCSI Block Commands - 3 (SBC-3) Revision 25
//   http://www.13thmonkey.org/documentation/SCSI/sbc3r25.pdf
//
//   [a] SCSI Architecture Model - 3 (SAM-3) Revision 13
//   http://www.csit-sun.pub.ro/~cpop/Documentatie_SMP/Standarde_magistrale/SCSI/sam3r13.pdf
//
//   [c] ATA Packet Interface for CD-ROMs Revision 2.6 Proposed
//   http://www.bswd.com/sff8020i.pdf
//
//   [s] SCSI Commands Reference Manual 100293068, Rev. J
//   https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification. References are prefixed by
// the letter in brackets as listed above.
#include "strikebox/hw/atapi/atapi_common.h"

namespace strikebox {
namespace hw {
namespace atapi {

PacketCommandState::DataBuffer::~DataBuffer() {
    if (m_buf != nullptr) {
        delete[] m_buf;
    }
}

bool PacketCommandState::DataBuffer::Allocate(uint32_t size) {
    // Should allocate only once
    assert(m_buf == nullptr);

    m_buf = new uint8_t[size];
    if (m_buf == nullptr) {
        return false;
    }
    m_size = 0;
    m_cap = size;
    Clear();
    return true;
}

uint32_t PacketCommandState::DataBuffer::Read(void *dst, uint32_t length) {
    assert(m_buf != nullptr);

    // Truncate to data length
    if (length + m_readPos > m_size) {
        length = m_size - m_readPos;
    }
    memcpy(dst, m_buf + m_readPos, length);
    m_readPos += length;
    return length;
}

uint32_t PacketCommandState::DataBuffer::Write(void *src, uint32_t length) {
    assert(m_buf != nullptr);

    // Truncate to allocation length
    if (length + m_writePos > m_cap) {
        length = m_cap - m_writePos;
    }
    memcpy(m_buf, src, length);
    m_size = length;
    return length;
}

}
}
}
