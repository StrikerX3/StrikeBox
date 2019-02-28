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

#include "proto_data_in.h"  

namespace strikebox {
namespace hw {
namespace atapi {
namespace cmd {

/*!
 * Implements the READ DVD STRUCTURE command (0xAD) [m 5.20].
 */
class ReadDVDStructure : public ATAPIDataInCommand {
public:
    ReadDVDStructure(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver);
    virtual ~ReadDVDStructure();

    bool BeginTransfer() override;

    // This command only transfer one block; Execute() will never be invoked
    bool Execute() override { return false; }

    static IATAPICommand *Factory(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver) { return new ReadDVDStructure(packetCmdState, driver); }

protected:
    uint32_t GetAllocationLength(CommandDescriptorBlock *cdb) override;
};

}
}
}
}
