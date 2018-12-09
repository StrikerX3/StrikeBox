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
#include "drv_vdvd_base.h"

#include "vixen/log.h"
#include "vixen/io.h"
#include "vixen/hw/ata/atapi_xbox.h"
#include "vixen/hw/ata/atapi_utils.h"

namespace vixen {
namespace hw {
namespace ata {

using namespace atapi;

BaseDVDDriveATADeviceDriver::BaseDVDDriveATADeviceDriver() {
}

BaseDVDDriveATADeviceDriver::~BaseDVDDriveATADeviceDriver() {
}

bool BaseDVDDriveATADeviceDriver::Read(uint64_t byteAddress, uint8_t * buffer, uint32_t size) {
    // Always fail; this function should never be called on devices that support the PACKET Command feature set
    log_warning("BaseDVDDriveATADeviceDriver::Read:  Unexpected ATA read\n");
    return false;
}

bool BaseDVDDriveATADeviceDriver::Write(uint64_t byteAddress, uint8_t * buffer, uint32_t size) {
    // Always fail; this function should never be called on devices that support the PACKET Command feature set
    log_warning("BaseDVDDriveATADeviceDriver::Write:  Unexpected ATA write\n");
    return false;
}

void BaseDVDDriveATADeviceDriver::IdentifyDevice(IdentifyDeviceData *data) {
    // Should never be invoked since this device supports the PACKET Command feature set
    // Fill with zeros just in case
    memset(data, 0, sizeof(IdentifyDeviceData));
}

bool BaseDVDDriveATADeviceDriver::IdentifyPacketDevice(IdentifyPacketDeviceData *data) {
    memset(data, 0, sizeof(IdentifyPacketDeviceData));

    // Adapted from https://github.com/mirror/vbox/blob/master/src/VBox/Devices/Storage/DevATA.cpp
    
    // Removable CDROM, 3ms response, 12 byte packets
    data->generalConfiguration = (IDPGenConfDTATAPIDevice << IDPGenConfDeviceTypeShift)
        | (IDPGenConfCPSCDROM << IDPGenConfCommandPacketSetShift)
        | IDPGenConfRemovableMedia
        | (IDPGenConfPRSetDRQIn3ms << IDPGenConfPACKETResponseShift)
        | (IDPGenConfCS12Byte << IDPGenConfCommandSizeShift);

    padString((uint8_t *)data->serialNumber, m_serialNumber, kSerialNumberLength);
    padString((uint8_t *)data->firmwareRevision, m_firmwareRevision, kFirmwareRevLength);
    padString((uint8_t *)data->modelNumber, m_modelNumber, kModelNumberLength);

    data->capabilities = IDPCapsDMA | IDPCapsLBA | IDPCapsIORDYSupported;
    data->validTranslationFields = IDValidXlatTransferCycles | IDValidXlatUltraDMA;

    data->multiwordDMASettings = IDMultiwordDMA2Supported | IDMultiwordDMA1Supported | IDMultiwordDMA0Supported;
    if (m_dmaTransferType == XferTypeMultiWordDMA) {
        data->multiwordDMASettings |= IDMultiwordDMA0Selected << m_dmaTransferMode;
    }

    data->advancedPIOModesSupported = 2; // Up to PIO mode 4
    data->minMDMATransferCyclePerWord = 120;
    data->recommendedMDMATransferCycleTime = 120;
    data->minPIOTransferCycleNoFlowCtl = 120;
    data->minPIOTransferCycleIORDYFlowCtl = 120;

    data->majorVersionNumber = IDMajorVerATAPI4 | IDMajorVerATA3 | IDMajorVerATA2 | IDMajorVerATA1;
    data->minorVersionNumber = IDMinorVerATAPI4_T13_1153D_rev17;

    data->queueDepth = 1;

    data->commandSetsSupported1 = IDCmdSet1PACKETCommandFeatureSet | IDCmdSet1DeviceReset;
    data->commandSetsSupported2 = IDCmdSet2Bit14AlwaysOne;
    data->commandSetsSupported3 = IDCmdSet3Bit14AlwaysOne;

    data->commandSetsEnabled1 = IDCmdSet1PACKETCommandFeatureSet | IDCmdSet1DeviceReset;
    data->commandSetsEnabled2 = 0;
    data->commandSetsEnabled3 = IDCmdSet3Bit14AlwaysOne;

    data->ultraDMASettings = IDUltraDMA0Supported | IDUltraDMA1Supported | IDUltraDMA2Supported;
    if (m_dmaTransferType == XferTypeUltraDMA) {
        data->ultraDMASettings |= IDUltraDMA0Selected << m_dmaTransferMode;
    }

    return true;
}

bool BaseDVDDriveATADeviceDriver::SecurityUnlock(uint8_t unlockData[kSectorSize]) {
    // We don't really care if the unlock data is correct or even valid; just unlock the drive
    return true;
}

bool BaseDVDDriveATADeviceDriver::SetDeviceParameters(uint8_t heads, uint8_t sectorsPerTrack) {
    // [8.16.2]: "Use prohibited for devices implementing the PACKET Command feature set."
    return false;
}

bool BaseDVDDriveATADeviceDriver::IsLBAAddressUserAccessible(uint32_t lbaAddress) {
    // Used by DMA protocol, which includes the Read DMA and Write DMA commands.
    // According to [8.23.2] and [8.45.2], devices implementing the PACKE
    // feature set are prohibited from using these commands. Therefore, this
    // method is not going to be used.
    log_warning("BaseDVDDriveATADeviceDriver::IsLBAAddressUserAccessible:  Unexpected DMA transfer!\n");
    return false;
}

uint32_t BaseDVDDriveATADeviceDriver::CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) {
    // Used by DMA protocol, which includes the Read DMA and Write DMA commands.
    // According to [8.23.2] and [8.45.2], devices implementing the PACKE
    // feature set are prohibited from using these commands. Therefore, this
    // method is not going to be used.
    log_warning("BaseDVDDriveATADeviceDriver::CHSToLBA:  Unexpected DMA transfer!\n");
    return 0;
}

void BaseDVDDriveATADeviceDriver::LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) {
    // Used by DMA protocol, which includes the Read DMA and Write DMA commands.
    // According to [8.23.2] and [8.45.2], devices implementing the PACKE
    // feature set are prohibited from using these commands. Therefore, this
    // method is not going to be used.
    log_warning("BaseDVDDriveATADeviceDriver::LBAToCHS:  Unexpected DMA transfer!\n");
}

uint8_t BaseDVDDriveATADeviceDriver::GetPacketCommandSize() {
    // Match the value specified in the IdentifyPacketDeviceData struct
    return 12;
}

bool BaseDVDDriveATADeviceDriver::ValidateCommand(PacketInformation& packetInfo) {
    switch (packetInfo.cdb.opCode.u8) {
    case OpModeSense10:
        switch (packetInfo.cdb.modeSense10.pageCode) {
        case kPageCodeAuthentication:
            // TODO: is it correct to fail if the length is smaller than the page data?
            if (B2L16(packetInfo.cdb.modeSense10.length) < sizeof(XboxDVDAuthentication)) {
                packetInfo.result.aborted = true;
                packetInfo.result.incorrectLength = true;
                packetInfo.result.status = StCheckCondition;
                packetInfo.result.senseKey = SKIllegalRequest;
                packetInfo.result.additionalSenseCode = ASCInvalidFieldInCDB;
                return false;
            }
            packetInfo.transferSize = sizeof(XboxDVDAuthentication);
            return true;
        }
        return true;
    case OpRequestSense:
        packetInfo.transferSize = packetInfo.cdb.requestSense.length;
        return true;
    case OpReadCapacity:
        packetInfo.transferSize = sizeof(ReadCapacityData);
        return true;
    case OpRead10:
        packetInfo.transferSize = (uint32_t)B2L16(packetInfo.cdb.read10.length) * kDVDSectorSize;
        return true;
    case OpReadDVDStructure:
        packetInfo.transferSize = kDVDSectorSize;
        return true;
    default:
        return true;
    }
}

}
}
}
