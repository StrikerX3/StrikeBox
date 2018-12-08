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

namespace vixen {
namespace hw {
namespace ata {

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

bool DummyDVDDriveATADeviceDriver::ValidateATAPIPacket(atapi::PacketInformation& packetInfo) {
    log_debug("DummyDVDDriveATADeviceDriver::ValidateATAPIPacket:  Operation Code 0x%x  ->  Command Code 0x%x  Group Code 0x%x\n", packetInfo.cdb.opCode.u8, packetInfo.cdb.opCode.fields.commandCode, packetInfo.cdb.opCode.fields.groupCode);
    
    // TODO: device-specific validation
    
    // Check if the command is supported and has valid parameters.
    return ValidateCommand(packetInfo);
}

bool DummyDVDDriveATADeviceDriver::ProcessATAPIPacketNonData(atapi::PacketInformation& packetInfo) {
    log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketNonData:  Operation Code 0x%x  ->  Command Code 0x%x  Group Code 0x%x\n", packetInfo.cdb.opCode.u8, packetInfo.cdb.opCode.fields.commandCode, packetInfo.cdb.opCode.fields.groupCode);
  
    // TODO: implement
    return false;
}

bool DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead(atapi::PacketInformation& packetInfo, uint8_t *packetDataBuffer, uint16_t byteCountLimit, uint32_t *packetDataSize) {
    log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataRead:  Operation Code 0x%x  ->  Command Code 0x%x  Group Code 0x%x   byte count limit = 0x%x\n", packetInfo.cdb.opCode.u8, packetInfo.cdb.opCode.fields.commandCode, packetInfo.cdb.opCode.fields.groupCode, byteCountLimit);
 
    // TODO: implement
    return false;
}

bool DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataWrite(atapi::PacketInformation& packetInfo, uint8_t *packetDataBuffer, uint16_t byteCountLimit) {
    log_debug("DummyDVDDriveATADeviceDriver::ProcessATAPIPacketDataWrite:  Operation Code 0x%x  ->  Command Code 0x%x  Group Code 0x%x   byte count limit = 0x%x\n", packetInfo.cdb.opCode.u8, packetInfo.cdb.opCode.fields.commandCode, packetInfo.cdb.opCode.fields.groupCode, byteCountLimit);

    // TODO: implement
    return false;
}

}
}
}
