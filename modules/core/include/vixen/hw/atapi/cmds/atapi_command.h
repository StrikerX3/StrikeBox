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

#include "../atapi_common.h"
#include "../atapi_utils.h"
#include "../atapi_xbox.h"
#include "vixen/hw/ata/drvs/ata_device_driver.h"

namespace vixen {
namespace hw {
namespace atapi {
namespace cmd {

/*!
 * Interface for ATAPI commands.
 *
 * Classes that derive this interface provide base implementations for ATAPI
 * protocols, which handle the basic flow of the packet command execution.
 */
class IATAPICommand {
public:
    IATAPICommand(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver);
    virtual ~IATAPICommand();

    /*!
     * Validates input parameters and allocates a data transfer buffer if
     * required by the command. If the command transfers data to the host,
     * the buffer is pre-filled with the necessary data.
     *
     * Status (including sense key and additional sense code) is updated in
     * the packet command state.
     *
     * Returns true if the packet data has been successfully validated and
     * the command is ready to be executed or to receive data.
     */
    virtual bool Prepare() = 0;

    /*!
     * Executes the command with the provided state.
     */
    virtual bool Execute() = 0;

    /*!
     * Determines if the data transfer has finished.
     */
    virtual bool IsTransferFinished() = 0;

    /*!
     * Retrieves the operation type for this command.
     */
    virtual PacketOperationType GetOperationType() = 0;

    /*!
     * Defines the factory function type used to build a factory table.
     */
    typedef IATAPICommand* (*Factory)(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver);

protected:
    PacketCommandState& m_packetCmdState;
    ata::IATADeviceDriver *m_driver;
};

}
}
}
}
