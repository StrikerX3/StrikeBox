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
#include "strikebox/hw/atapi/cmds/cmd_test_unit_ready.h"

#include "strikebox/log.h"

namespace strikebox {
namespace hw {
namespace atapi {
namespace cmd {

TestUnitReady::TestUnitReady(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver)
    : ATAPINonDataCommand(packetCmdState, driver)
{
}

TestUnitReady::~TestUnitReady() {
}

bool TestUnitReady::Prepare() {
    // Nothing to prepare or validate
    return true;
}

bool TestUnitReady::Execute() {
    // If there is no disc in the drive, return the expected sense key and parameters.
    // The default values tell that there is media in the drive and the device is ready to accept commands.
    if (!m_driver->HasMedium()) {
        m_packetCmdState.result.status = StCheckCondition;
        m_packetCmdState.result.senseKey = SKNotReady;
        m_packetCmdState.result.additionalSenseCode = ASCMediumNotPresent;
    }
    return true;
}

}
}
}
}
