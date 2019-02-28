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
#include "strikebox/hw/atapi/cmds/cmd_read_dvd_structure.h"

#include "strikebox/log.h"

namespace strikebox {
namespace hw {
namespace atapi {
namespace cmd {

ReadDVDStructure::ReadDVDStructure(PacketCommandState& packetCmdState, ata::IATADeviceDriver *driver)
    : ATAPIDataInCommand(packetCmdState, driver)
{
}

ReadDVDStructure::~ReadDVDStructure() {
}

bool ReadDVDStructure::BeginTransfer() {
    auto& cdb = m_packetCmdState.cdb.readDVDStructure;
    ReadDVDStructureData dvdData = { 0 };
    switch (cdb.format) {
    case DVDFmtPhysical:
        if (m_driver->HasMedium()) {
            L2B16(dvdData.dataLength, 2048+2/*(uint16_t)sizeof(ReadDVDStructureData::physicalFormatInformation)*/);
        
            // TODO: Compute fields based on the data reported by the driver
            // Or perhaps let the driver fill it in?
            dvdData.physicalFormatInformation.partVersion = 1;
            dvdData.physicalFormatInformation.bookType = BookTypeDVDROM;
            dvdData.physicalFormatInformation.maxRate = MaxRate10_08Mbps;
            dvdData.physicalFormatInformation.discSize = DiscSize120mm;
            dvdData.physicalFormatInformation.layerType = LayerTypeEmbossed;
            dvdData.physicalFormatInformation.trackPath = TrackPathOTP;
            dvdData.physicalFormatInformation.numLayers = NumLayers2;
            dvdData.physicalFormatInformation.trackDensity = TrackDensity0_74umPerTrack;
            dvdData.physicalFormatInformation.linearDensity = LinearDensity0_293umPerBit;
            L2B24(dvdData.physicalFormatInformation.dataStartingSector, kStartingSectorNumberDVDROM);
            L2B24(dvdData.physicalFormatInformation.dataEndingSector, m_driver->GetMediumCapacitySectors());
            L2B24(dvdData.physicalFormatInformation.layer0EndingSector, m_driver->GetMediumCapacitySectors());
            dvdData.physicalFormatInformation.burstCuttingArea = 0;
        }
        else {
            L2B16(dvdData.dataLength, 0);
            m_packetCmdState.result.status = StCheckCondition;
            m_packetCmdState.result.senseKey = SKIllegalRequest;
            m_packetCmdState.result.additionalSenseCode = ASCMediumNotPresent;
        }
        
        m_packetCmdState.dataBuffer.Write(&dvdData, sizeof(dvdData));
        EndTransfer();
        
        return true;
    default:
        log_debug("ImageDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead:  Unimplemented format 0x%x for READ DVD STRUCTURE\n", cdb.format);
        return false;
    }
}

uint32_t ReadDVDStructure::GetAllocationLength(CommandDescriptorBlock *cdb) {
    return B2L16(cdb->readDVDStructure.allocLength);
}

}
}
}
}
