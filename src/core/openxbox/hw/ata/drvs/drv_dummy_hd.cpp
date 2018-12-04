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
#include "drv_dummy_hd.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {
namespace hw {
namespace ata {

DummyHardDriveATADeviceDriver g_dummyATADeviceDriver;

static void padString(uint8_t *dest, const char *src, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (*src) {
            dest[i ^ 1] = *src++;
        }
        else {
            dest[i ^ 1] = ' ';
        }
    }
}

void DummyHardDriveATADeviceDriver::IdentifyDevice(IdentifyDeviceData *data) {
    // Fill in with reasonable parameters for a 10 GB hard drive
    memset(data, 0, sizeof(IdentifyDeviceData));

    // Adapted from https://github.com/mirror/vbox/blob/master/src/VBox/Devices/Storage/DevATA.cpp
    data->generalConfiguration = IDGenConfATADevice;
    data->numLogicalCylinders = 5120;
    data->numLogicalHeads = 15;
    data->numLogicalSectorsPerTrack = 255;
    
    padString((uint8_t *)data->serialNumber, "1234567890", kSerialNumberLength);
    padString((uint8_t *)data->firmwareRevision, "1.00", kFirmwareRevLength);
    padString((uint8_t *)data->modelNumber, "DMY987654321", kModelNumberLength);
    
    data->validTranslationFields = IDValidXlatUltraDMA | IDValidXlatTransferCycles | IDValidXlatCHS;
    
    data->numCurrentLogicalCylinders = data->numLogicalCylinders;
    data->numCurrentLogicalHeads = data->numLogicalHeads;
    data->numCurrentLogicalSectorsPerTrack = data->numLogicalSectorsPerTrack;
    data->currentSectorCapacity = data->numCurrentLogicalCylinders * data->numCurrentLogicalHeads * data->numCurrentLogicalSectorsPerTrack;
    data->numAddressableSectors = data->currentSectorCapacity;

    data->multiwordDMASettings = IDMultiwordDMA0Supported | IDMultiwordDMA1Supported | IDMultiwordDMA2Supported | IDMultiwordDMA0Selected;
    data->advancedPIOModesSupported = 2; // Up to PIO mode 4
    data->minMDMATransferCyclePerWord = 120;
    data->recommendedMDMATransferCycleTime = 120;
    data->minPIOTransferCycleNoFlowCtl = 120;
    data->minPIOTransferCycleIORDYFlowCtl = 120;
    
    data->majorVersionNumber = IDMajorVerATAPI4 | IDMajorVerATA3 | IDMajorVerATA2 | IDMajorVerATA1;
    data->minorVersionNumber = IDMinorVerATAPI4_T13_1153D_rev17;
    
    data->commandSetsSupported1 = IDCmdSet1PowerMgmtFeatureSet | IDCmdSet1WriteCache | IDCmdSet1LookAhead;
    data->commandSetsSupported2 = IDCmdSet2Bit14AlwaysOne;
    data->commandSetsSupported3 = IDCmdSet3Bit14AlwaysOne;

    data->commandSetsEnabled1 = IDCmdSet1PowerMgmtFeatureSet | IDCmdSet1WriteCache | IDCmdSet1LookAhead;
    data->commandSetsEnabled2 = IDCmdSet2Bit14AlwaysOne;
    data->commandSetsEnabled3 = IDCmdSet3Bit14AlwaysOne;
    
    data->ultraDMASettings = IDUltraDMA0Supported | IDUltraDMA1Supported | IDUltraDMA2Supported;

    // Xbox hard drive must be locked
    data->securityStatus |= IDSecStatusSupported | IDSecStatusEnabled | IDSecStatusLocked;
}

}
}
}
