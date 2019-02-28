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
#pragma once

#include <cstdint>

#include "atapi_command.h"

namespace strikebox {
namespace hw {
namespace atapi {
namespace cmd {

/*!
 * Base class for all ATAPI commands that transfer data to the host.
 */
class ATAPIDataInCommand : public IATAPICommand {
public:
    ATAPIDataInCommand(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver);
    virtual ~ATAPIDataInCommand();

    bool Prepare() override;

    /*!
     * Reads data into the buffer.
     *
     * Returns true if the data retrieval succeeded, or false if there was any error.
     */
    virtual bool Execute() override = 0;

    PacketOperationType GetOperationType() override { return PktOpDataIn; }

    bool IsTransferFinished() override { return m_transferFinished; }

protected:
    /*!
     * Retrieves the allocation length from the CDB. Each command has its own
     * CDB structure, and the allocation length is not necessarily in the same
     * location or has the same size.
     */
    virtual uint32_t GetAllocationLength(CommandDescriptorBlock *cdb) = 0;

    /*!
     * Initializes the data transfer.
     */
    virtual bool BeginTransfer() = 0;

    /*!
     * Marks the end of a transfer.
     */
    void EndTransfer() { m_transferFinished = true; }

private:

    bool m_transferFinished = false;
};

}
}
}
}
