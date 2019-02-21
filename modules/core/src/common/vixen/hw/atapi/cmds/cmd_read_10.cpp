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
#include "vixen/hw/atapi/cmds/cmd_read_10.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace atapi {
namespace cmd {

Read10::Read10(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver)
    : ATAPIDataInCommand(packetCmdState, driver)
{
}

Read10::~Read10() {
    if (m_buffer != nullptr) {
        delete[] m_buffer;
    }
}

bool Read10::BeginTransfer() {
    auto& cdb = m_packetCmdState.cdb.read10;
    if (!m_driver->HasMedium()) {
        // There is no medium in the drive
        m_packetCmdState.result.status = StCheckCondition;
        m_packetCmdState.result.senseKey = SKNotReady;
        m_packetCmdState.result.additionalSenseCode = ASCMediumNotPresent;
        return true;
    }

    uint32_t lba = B2L32(cdb.lba);
    uint16_t transferLength = B2L16(cdb.transferLength);
    uint32_t transferLengthBytes = (uint32_t)transferLength * m_driver->GetSectorSize();

    m_transferLength = transferLengthBytes;
    if (m_transferLength > m_packetCmdState.input.byteCountLimit) {
        m_transferLength = m_packetCmdState.input.byteCountLimit;
    }

    // Allocate buffer and setup transfer parameters
    m_currentByte = lba * m_driver->GetSectorSize();
    m_lastByte = m_currentByte + transferLengthBytes;
    m_buffer = new uint8_t[m_transferLength];
    
    //log_spew("Read10::BeginTransfer:  Starting transfer: 0x%llx to 0x%llx\n", m_currentByte, m_lastByte);

    // Read from media
    return Execute();
}

bool Read10::Execute() {
    uint32_t readLen = m_lastByte - m_currentByte;
    if (readLen > m_transferLength) {
        readLen = m_transferLength;
    }

    if (!m_driver->HasMedium()) {
        // No medium in drive
        m_packetCmdState.result.status = StCheckCondition;
        m_packetCmdState.result.senseKey = SKIllegalRequest;
        m_packetCmdState.result.additionalSenseCode = ASCMediumNotPresent;
        log_spew("Read10::Execute:  Medium not present\n");
        EndTransfer();
        return false;
    }

    // Read from the device
    // TODO: maybe handle caching? Could improve performance if accessing real media on supported drives
    // Should also honor the cache flags
    bool successful = m_driver->Read(m_currentByte, m_buffer, readLen);

    if (!successful) {
        // Reached the end of the medium
        m_packetCmdState.result.status = StCheckCondition;
        m_packetCmdState.result.senseKey = SKNoSense;
        m_packetCmdState.result.additionalSenseCode = ASCEndOfMediumReached;
        m_packetCmdState.result.endOfMedium = true;
        log_spew("Read10::Execute:  Reached end of medium\n");
        EndTransfer();
        return false;
    }

    // Update position and check if the transfer ended
    m_currentByte += readLen;
    if (m_currentByte >= m_lastByte) {
        //log_spew("Read10::Execute:  Transfer finished\n");
        EndTransfer();
    }

    // Write to the transfer buffer
    m_packetCmdState.dataBuffer.Clear();
    m_packetCmdState.dataBuffer.Write(m_buffer, readLen);

    return true;
}

uint32_t Read10::GetAllocationLength(CommandDescriptorBlock *cdb) {
    return B2L16(cdb->read10.transferLength) * m_driver->GetSectorSize();
}

}
}
}
}
