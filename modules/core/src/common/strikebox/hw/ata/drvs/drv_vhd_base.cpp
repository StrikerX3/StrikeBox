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
#include "strikebox/hw/ata/drvs/drv_vhd_base.h"

#include "strikebox/log.h"
#include "strikebox/io.h"

namespace strikebox {
namespace hw {
namespace ata {

BaseHardDriveATADeviceDriver::BaseHardDriveATADeviceDriver() 
    : m_numLogicalCylinders(0)
    , m_numLogicalHeads(0)
    , m_numLogicalSectorsPerTrack(0)
    , m_locked(true)
{
}

BaseHardDriveATADeviceDriver::~BaseHardDriveATADeviceDriver() {
}

void BaseHardDriveATADeviceDriver::IdentifyDevice(IdentifyDeviceData *data) {
    memset(data, 0, sizeof(IdentifyDeviceData));

    // Adapted from https://github.com/mirror/vbox/blob/master/src/VBox/Devices/Storage/DevATA.cpp
    data->generalConfiguration = IDGenConfATADevice;
    data->numLogicalCylinders = m_numCylinders;
    data->numLogicalHeads = m_numHeadsPerCylinder;
    data->numLogicalSectorsPerTrack = m_numSectorsPerTrack;

    padString((uint8_t *)data->serialNumber, m_serialNumber, kSerialNumberLength);
    padString((uint8_t *)data->firmwareRevision, m_firmwareRevision, kFirmwareRevLength);
    padString((uint8_t *)data->modelNumber, m_modelNumber, kModelNumberLength);

    data->validTranslationFields = IDValidXlatUltraDMA | IDValidXlatTransferCycles;
    
    uint32_t currentSectorCapacity = m_numLogicalCylinders * m_numLogicalHeads * m_numLogicalSectorsPerTrack;
    if (currentSectorCapacity != 0) {
        data->validTranslationFields |= IDValidXlatCHS;
    }

    data->numCurrentLogicalCylinders = m_numLogicalCylinders;
    data->numCurrentLogicalHeads = m_numLogicalHeads;
    data->numCurrentLogicalSectorsPerTrack = m_numLogicalSectorsPerTrack;
    data->currentSectorCapacity = currentSectorCapacity;
    data->numAddressableSectors = m_sectorCapacity;

    data->multiwordDMASettings = IDMultiwordDMA0Supported | IDMultiwordDMA1Supported | IDMultiwordDMA2Supported;
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

    data->commandSetsSupported1 = IDCmdSet1PowerMgmtFeatureSet | IDCmdSet1WriteCache | IDCmdSet1LookAhead;
    data->commandSetsSupported2 = IDCmdSet2Bit14AlwaysOne;
    data->commandSetsSupported3 = IDCmdSet3Bit14AlwaysOne;

    data->commandSetsEnabled1 = IDCmdSet1PowerMgmtFeatureSet | IDCmdSet1WriteCache | IDCmdSet1LookAhead;
    data->commandSetsEnabled2 = IDCmdSet2Bit14AlwaysOne;
    data->commandSetsEnabled3 = IDCmdSet3Bit14AlwaysOne;

    data->ultraDMASettings = IDUltraDMA0Supported | IDUltraDMA1Supported | IDUltraDMA2Supported;
    if (m_dmaTransferType == XferTypeUltraDMA) {
        data->ultraDMASettings |= IDUltraDMA0Selected << m_dmaTransferMode;
    }

    // Xbox hard drive must support security features and have them enabled.
    // The hard drive doesn't necessarily need to be locked.
    data->securityStatus |= IDSecStatusSupported | IDSecStatusEnabled;
    if (m_locked) {
        data->securityStatus |= IDSecStatusLocked;
    }
}

bool BaseHardDriveATADeviceDriver::IdentifyPacketDevice(IdentifyPacketDeviceData *data) {
    return false;
}

bool BaseHardDriveATADeviceDriver::SecurityUnlock(uint8_t unlockData[kSectorSize]) {
    // We don't really care if the unlock data is correct or even valid; just unlock the drive
    m_locked = false;
    return true;
}

bool BaseHardDriveATADeviceDriver::SetDeviceParameters(uint8_t heads, uint8_t sectorsPerTrack) {
    // Fail if the requested number of heads or sectors per track exceed the parameters of the drive
    if (heads > m_numHeadsPerCylinder || sectorsPerTrack > m_numSectorsPerTrack) {
        return false;
    }

    // Compute number of cylinders
    uint32_t chsSectorCapacity = (m_sectorCapacity < kMaxCHSSectorCapacity) ? m_sectorCapacity : kMaxCHSSectorCapacity;
    uint32_t numCylinders = chsSectorCapacity / ((heads + 1) * sectorsPerTrack);
    if (numCylinders > 65535) {
        numCylinders = 65535;
    }

    m_numLogicalSectorsPerTrack = sectorsPerTrack;
    m_numLogicalHeads = heads + 1;
    m_numLogicalCylinders = numCylinders;
    return true;
}

bool BaseHardDriveATADeviceDriver::IsLBAAddressUserAccessible(uint32_t lbaAddress) {
    return lbaAddress <= m_sectorCapacity;
}

uint32_t BaseHardDriveATADeviceDriver::CHSToLBA(uint32_t cylinder, uint8_t head, uint8_t sector) {
    return ((cylinder * m_numLogicalHeads) + head) * m_numLogicalSectorsPerTrack + sector;
}

void BaseHardDriveATADeviceDriver::LBAToCHS(uint32_t lbaAddress, uint16_t *cylinder, uint8_t *head, uint8_t *sector) {
    *sector = lbaAddress % m_numLogicalSectorsPerTrack;
    lbaAddress /= m_numLogicalSectorsPerTrack;

    *head = lbaAddress % m_numLogicalHeads;
    lbaAddress /= m_numLogicalHeads;

    *cylinder = lbaAddress;
}

}
}
}
