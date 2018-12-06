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

namespace vixen {
namespace hw {
namespace ata {

BaseDVDDriveATADeviceDriver::BaseDVDDriveATADeviceDriver() {
}

BaseDVDDriveATADeviceDriver::~BaseDVDDriveATADeviceDriver() {
}

void BaseDVDDriveATADeviceDriver::IdentifyDevice(IdentifyDeviceData *data) {
    // Should never be invoked since this device supports the PACKET Command feature set
    // Fill with zeros just in case
    memset(data, 0, sizeof(IdentifyDeviceData));
}

bool BaseDVDDriveATADeviceDriver::IdentifyPACKETDevice(IdentifyPACKETDeviceData *data) {
    memset(data, 0, sizeof(IdentifyPACKETDeviceData));

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
    data->multiwordDMASettings = IDMultiwordDMA2Supported | IDMultiwordDMA1Supported | IDMultiwordDMA0Supported | IDMultiwordDMA0Selected;
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
    
    return true;
}

bool BaseDVDDriveATADeviceDriver::SecurityUnlock(uint8_t unlockData[kSectorSize]) {
    // We don't really care if the unlock data is correct or even valid; just unlock the drive
    return true;
}

bool BaseDVDDriveATADeviceDriver::SetDeviceParameters(uint8_t heads, uint8_t sectorsPerTrack) {
    // TODO: implement
    return true;
}

bool BaseDVDDriveATADeviceDriver::IsLBAAddressUserAccessible(uint32_t lbaAddress) {
    // TODO: implement
    return false;
}

uint32_t BaseDVDDriveATADeviceDriver::CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) {
    // TODO: implement
    return 0;
}

void BaseDVDDriveATADeviceDriver::LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) {
    // TODO: implement
}

}
}
}
