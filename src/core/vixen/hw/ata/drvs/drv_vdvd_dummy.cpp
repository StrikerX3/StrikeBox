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
#include "drv_vdvd_dummy.h"

#include "vixen/log.h"
#include "vixen/io.h"
#include "vixen/hw/ata/atapi_defs.h"
#include "vixen/hw/ata/atapi_xbox.h"
#include "vixen/hw/ata/atapi_utils.h"

namespace vixen {
namespace hw {
namespace ata {

using namespace atapi;

DummyDVDDriveATADeviceDriver::DummyDVDDriveATADeviceDriver() {
    strcpy(m_serialNumber, "1234567890");
    strcpy(m_firmwareRevision, "1.00");
    strcpy(m_modelNumber, "DUMMY DVD 12345");
}

DummyDVDDriveATADeviceDriver::~DummyDVDDriveATADeviceDriver() {
}

bool DummyDVDDriveATADeviceDriver::Read(uint64_t byteAddress, uint8_t *buffer, uint32_t size) {
    // Fill with zeros, as if the disk was blank
    memset(buffer, 0, size);

    // Always succeed
    return true;
}

bool DummyDVDDriveATADeviceDriver::Write(uint64_t byteAddress, uint8_t *buffer, uint32_t size) {
    // Lie about writing, always fail
    return false;
}

bool DummyDVDDriveATADeviceDriver::ValidateATAPIPacket(PacketInformation& packetInfo) {
    log_debug("DummyDVDDriveATADeviceDriver::ValidateATAPIPacket:  Operation code 0x%x\n", packetInfo.cdb.opCode.u8, packetInfo.cdb.opCode.fields.commandCode, packetInfo.cdb.opCode.fields.groupCode);
    
    // TODO: device-specific validation
    
    // Check if the command is supported and has valid parameters.
    return ValidateCommand(packetInfo);
}

bool DummyDVDDriveATADeviceDriver::ProcessATAPIPacketNonData(PacketInformation& packetInfo) {
    switch (packetInfo.cdb.opCode.u8) {
    case OpTestUnitReady:
        // Say that there is no disc in the drive
        packetInfo.result.status = StCheckCondition;
        packetInfo.result.senseKey = SKNotReady;
        packetInfo.result.additionalSenseCode = ASCMediumNotPresent;
        return true;
    default:
        log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketNonData:  Unimplemented operation code 0x%x\n", packetInfo.cdb.opCode.u8);
        return false;
    }
}

bool DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead(PacketInformation& packetInfo, uint8_t *packetDataBuffer, uint16_t byteCountLimit, uint32_t *packetDataSize) {
    switch (packetInfo.cdb.opCode.u8) {
    case OpModeSense10:
        switch (packetInfo.cdb.modeSense10.pageCode) {
        case kPageCodeAuthentication:
        {
            // TODO: handle partial reads
            if (byteCountLimit < sizeof(XboxDVDAuthentication)) {
                packetInfo.result.aborted = true;
                packetInfo.result.deviceFault = true;
                return false;
            }

            // Fill in just enough information to pass basic authentication checks on modified kernels
            // TODO: Research Xbox DVD authentication
            // https://multimedia.cx/eggs/xbox-sphinx-protocol/
            XboxDVDAuthentication *dvdAuth = reinterpret_cast<XboxDVDAuthentication *>(packetDataBuffer);
            dvdAuth->CDFValid = 1;
            dvdAuth->PartitionArea = 1;
            dvdAuth->Authentication = 1;
            
            *packetDataSize = sizeof(XboxDVDAuthentication);
            return true;
        }
        default:
            log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead:  Unimplemented page code 0x%x for MODE SENSE(10)\n", packetInfo.cdb.modeSense10.pageCode);
            return false;
        }
    case OpReadCapacity:
    {
        // Say that there is no disc in the drive
        packetInfo.result.status = StCheckCondition;
        packetInfo.result.senseKey = SKNotReady;
        packetInfo.result.additionalSenseCode = ASCMediumNotPresent;

        ReadCapacityData *capData = reinterpret_cast<ReadCapacityData *>(packetDataBuffer);
        L2B32(capData->lba, 0);
        L2B32(capData->blockLength, 0);

        *packetDataSize = sizeof(ReadCapacityData);
        return true;
    }
    default:
        log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead:  Unimplemented operation code 0x%x\n", packetInfo.cdb.opCode.u8);
        return false;
    }
}

bool DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataWrite(PacketInformation& packetInfo, uint8_t *packetDataBuffer, uint16_t byteCountLimit) {

    log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataWrite:  Unimplemented operation code 0x%x\n", packetInfo.cdb.opCode.u8);
    return false;
}

}
}
}
