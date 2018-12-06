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
#include "ata.h"

#include "vixen/log.h"
#include "vixen/io.h"

namespace vixen {
namespace hw {
namespace ata {

ATADevice::ATADevice(Channel channel, uint8_t devIndex, ATARegisters& regs)
    : m_channel(channel)
    , m_devIndex(devIndex)
    , m_driver(&g_nullATADeviceDriver)
    , m_regs(regs)
    , m_transferActive(false)
{
}

ATADevice::~ATADevice() {
}

uint32_t ATADevice::ReadBuffer(uint8_t *dst, uint32_t length) {
    uint32_t lenToRead = length;
    if (m_dataBufferPos + length > kSectorSize) {
        lenToRead = GetRemainingBufferLength();
    }

    memcpy(dst, m_dataBuffer + m_dataBufferPos, lenToRead);
    m_dataBufferPos += lenToRead;
    return lenToRead;
}

uint32_t ATADevice::WriteBuffer(uint8_t *src, uint32_t length) {
    uint32_t lenToWrite = length;
    if (m_dataBufferPos + length > kSectorSize) {
        lenToWrite = GetRemainingBufferLength();
    }

    memcpy(m_dataBuffer + m_dataBufferPos, src, lenToWrite);
    m_dataBufferPos += lenToWrite;
    return lenToWrite;
}

uint32_t ATADevice::GetRemainingBufferLength() {
    return kSectorSize - m_dataBufferPos;
}

bool ATADevice::IsBlockTransferComplete() {
    return GetRemainingBufferLength() == 0;
}

bool ATADevice::RequestNextBlock() {
    if (m_sectorsRemaining > 0) {
        ExecuteCommand();
        m_sectorsRemaining--;
        m_dataBufferPos = 0;
        return true;
    }
    return false;
}

bool ATADevice::BeginReadDMA() {
    // [8.23.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    // Sanity check: don't start a DMA transfer while another transfer is running
    if (m_transferActive) {
        return false;
    }

    // Determine if we're using LBA address or CHS numbers
    bool useLBA = (m_regs.deviceHead & DevHeadDMALBA) != 0;

    // Read address accordingly and validate parameters
    if (useLBA) {
        m_dma_startingLBA = ((m_regs.deviceHead & 0b1111) << 24) | (m_regs.cylinder << 8) | (m_regs.sectorNumber);
    }
    else {
        // Convert from CHS to LBA
        uint16_t cylinder = m_regs.cylinder;
        uint8_t head = m_regs.deviceHead & 0b1111;
        uint8_t sector = m_regs.sectorNumber;
        m_dma_startingLBA = m_driver->CHSToLBA(cylinder, head, sector);
    }

    // Calculate ending LBA
    if (m_regs.sectorCount == 0) {
        m_dma_endingLBA = m_dma_startingLBA + 256;
    }
    else {
        m_dma_endingLBA = m_dma_startingLBA + m_regs.sectorCount;
    }

    if (m_driver->IsLBAAddressUserAccessible(m_dma_startingLBA)) {
        m_transferActive = true;
        m_dma_isWrite = false;
        m_dma_currentLBA = m_dma_startingLBA;
        return true;
    }
    return false;
}

bool ATADevice::ReadDMA(uint8_t dstBuffer[kSectorSize]) {
    // [8.23.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    bool succeeded = HandleReadDMA(dstBuffer);

    if (succeeded) {
        // Handle normal output as specified in [8.23.5]

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller
       
        //  "DRDY shall be set to one."
        m_regs.status |= StReady;

        //  "DF(Device Fault) shall be cleared to zero."
        //  "DRQ shall be cleared to zero."
        //  "ERR shall be cleared to zero."
        m_regs.status &= ~(StDeviceFault | StDataRequest | StError);
    }
    else {
        // Handle error output as specified in [8.23.6]

        // Error register is handled by the HandleReadDMA function

        // Sector Number, Cylinder Low, Cylinder High, Device/Head:
        //  "shall be written with the address of first unrecoverable error."
        m_driver->LBAToCHS(m_dma_currentLBA, &m_regs.cylinder, &m_regs.sectorNumber, &m_regs.deviceHead);

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller

        //  "DRDY shall be set to one."
        m_regs.status |= StReady;

        //  "DF (Device Fault) shall be set to one if a device fault has occurred."
        //    Handled by HandleReadDMA

        //  "DRQ shall be cleared to zero."
        m_regs.status &= ~StDataRequest;

        //  "ERR shall be set to one if an Error register bit is set to one."
        if (m_regs.error) {
            m_regs.status |= StError;
        }
    }

    return succeeded;
}

bool ATADevice::HandleReadDMA(uint8_t dstBuffer[kSectorSize]) {
    // Sanity check: don't read while a DMA write is in progress
    if (m_dma_isWrite) {
        m_regs.status |= StDeviceFault;
        return false;
    }

    // Done reading
    if (IsDMAFinished()) {
        return false;
    }

    // If the device uses removable media, check if it has media
    // TODO: implement removable media functions in the driver
    /*if (m_driver->IsRemovableMedia() && !m_driver->HasMedia()) {
        // [8.23.6]: "NM shall be set to one if no media is present in a removable media device."
        m_regs.error |= ErrDMANoMedia;
        return false;
    }*/

    // Check that the removable media was not changed while the operation is in progress
    // TODO: implement removable media functions in the driver
    /*if (m_driver->MediaChangeRequested()) {
        // [8.23.6]: "MCR shall be set to one if a media change request has been detected by a removable media device."
        m_regs.error |= ErrDMAMediaChangeRequest;
        return false;
    }*/

    // Check that the next sector is accessible
    if (!m_driver->IsLBAAddressUserAccessible(m_dma_currentLBA)) {
        // [8.23.6]: "IDNF shall be set to one if a user-accessible address could not be found"
        m_regs.error |= ErrDMADataNotFound;
        return false;
    }

    // Try to read next sector
    if (!m_driver->ReadSector(m_dma_currentLBA, dstBuffer)) {
        m_regs.status |= StDeviceFault;
        return false;
    }

    m_dma_currentLBA++;

    return true;
}

bool ATADevice::BeginWriteDMA() {
    // [8.45.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    // Sanity check: don't start a DMA transfer while another transfer is running
    if (m_transferActive) {
        return false;
    }

    // Determine if we're using LBA address or CHS numbers
    bool useLBA = (m_regs.deviceHead & DevHeadDMALBA) != 0;

    // Read address accordingly and validate parameters
    if (useLBA) {
        m_dma_startingLBA = ((m_regs.deviceHead & 0b1111) << 24) | (m_regs.cylinder << 8) | (m_regs.sectorNumber);
    }
    else {
        // Convert from CHS to LBA
        uint16_t cylinder = m_regs.cylinder;
        uint8_t head = m_regs.deviceHead & 0b1111;
        uint8_t sector = m_regs.sectorNumber;
        m_dma_startingLBA = m_driver->CHSToLBA(cylinder, head, sector);
    }

    // Calculate ending LBA
    if (m_regs.sectorCount == 0) {
        m_dma_endingLBA = m_dma_startingLBA + 256;
    }
    else {
        m_dma_endingLBA = m_dma_startingLBA + m_regs.sectorCount;
    }

    if (m_driver->IsLBAAddressUserAccessible(m_dma_startingLBA)) {
        m_transferActive = true;
        m_dma_isWrite = true;
        m_dma_currentLBA = m_dma_startingLBA;
        return true;
    }
    return false;
}

bool ATADevice::WriteDMA(uint8_t srcBuffer[kSectorSize]) {
    // [8.45.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    bool succeeded = HandleWriteDMA(srcBuffer);

    if (succeeded) {
        // Handle normal output as specified in [8.45.5]

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller

        //  "DRDY shall be set to one."
        m_regs.status |= StReady;

        //  "DF(Device Fault) shall be cleared to zero."
        //  "DRQ shall be cleared to zero."
        //  "ERR shall be cleared to zero."
        m_regs.status &= ~(StDeviceFault | StDataRequest | StError);
    }
    else {
        // Handle error output as specified in [8.45.6]

        // Error register is handled by the HandleWriteDMA function

        // Sector Number, Cylinder Low, Cylinder High, Device/Head:
        //  "shall be written with the address of first unrecoverable error."
        m_driver->LBAToCHS(m_dma_currentLBA, &m_regs.cylinder, &m_regs.sectorNumber, &m_regs.deviceHead);

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller

        //  "DRDY shall be set to one."
        m_regs.status |= StReady;

        //  "DF (Device Fault) shall be set to one if a device fault has occurred."
        //    Handled by HandleReadDMA

        //  "DRQ shall be cleared to zero."
        m_regs.status &= ~StDataRequest;

        //  "ERR shall be set to one if an Error register bit is set to one."
        if (m_regs.error) {
            m_regs.status |= StError;
        }
    }

    return succeeded;

}

bool ATADevice::HandleWriteDMA(uint8_t srcBuffer[kSectorSize]) {
    // Sanity check: don't write while a DMA read is in progress
    if (!m_dma_isWrite) {
        m_regs.status |= StDeviceFault;
        return false;
    }

    // Done writing
    if (IsDMAFinished()) {
        return false;
    }

    // If the device uses removable media, check if it has media
    // TODO: implement removable media functions in the driver
    /*if (m_driver->IsRemovableMedia() && !m_driver->HasMedia()) {
        // [8.23.6]: "NM shall be set to one if no media is present in a removable media device."
        m_regs.error |= ErrDMANoMedia;
        return false;
    }*/

    // Check that the removable media was not changed while the operation is in progress
    // TODO: implement removable media functions in the driver
    /*if (m_driver->MediaChangeRequested()) {
        // [8.23.6]: "MCR shall be set to one if a media change request has been detected by a removable media device."
        m_regs.error |= ErrDMAMediaChangeRequest;
        return false;
    }*/

    // Check that the next sector is accessible
    if (!m_driver->IsLBAAddressUserAccessible(m_dma_currentLBA)) {
        // [8.23.6]: "IDNF shall be set to one if a user-accessible address could not be found"
        m_regs.error |= ErrDMADataNotFound;
        return false;
    }

    // Try to write next sector
    if (!m_driver->WriteSector(m_dma_currentLBA, srcBuffer)) {
        m_regs.status |= StDeviceFault;
        return false;
    }

    m_dma_currentLBA++;

    return true;
}

void ATADevice::EndDMA() {
    m_transferActive = false;
}

bool ATADevice::HasTransferError() {
    return m_transferError;
}

void ATADevice::FinishCommand() {
    m_transferActive = false;
}

bool ATADevice::IdentifyDevice() {
    // [8.12.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    // Check if the device supports the PACKET Command feature set
    if (m_driver->SupportsPACKETCommands()) {
        // Respond as specified in [8.12.5.2], which follows the
        // Signature and Persistence protocol [9.1]
        WriteSignature(true);
        m_regs.error |= ErrAbort;
        return false;
    }

    // Ask the device driver to identify itself
    m_dataBufferPos = 0;
    m_transferHasCommand = false;
    m_driver->IdentifyDevice(reinterpret_cast<IdentifyDeviceData *>(m_dataBuffer));

    // The documentation does not specify error output for this command when
    // the device does not support the PACKET Command feature set.
    // Assume this command never fails.

    // Handle normal output as specified in [8.12.5.1]
    
    // Device/Head register:
    //  "DEV shall indicate the selected device."
    //     Not necessary, but the spec says so
    m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

    // Status register:
    //  "BSY shall be cleared to zero indicating command completion."
    //     Already handled by the caller

    //  "DRDY shall be set to one."
    m_regs.status |= StReady;

    //  "DF (Device Fault) shall be cleared to zero."
    //  "DRQ shall be cleared to zero."
    //  "ERR shall be cleared to zero."
    m_regs.status &= ~(StDeviceFault | StDataRequest | StError);

    return true;
}

bool ATADevice::IdentifyPACKETDevice() {
    // Ask the device driver to identify itself
    m_dataBufferPos = 0;
    m_transferHasCommand = false;
    if (!m_driver->IdentifyPACKETDevice(reinterpret_cast<IdentifyPACKETDeviceData *>(m_dataBuffer))) {
        // The device does not implement the Identify PACKET Device command
        // Return command aborted as specified in the error output section [8.13.6]
        m_regs.error |= ErrAbort;
        return false;
    }

    // Handle normal output as specified in [8.13.5]

    // Device/Head register:
    //  "DEV shall indicate the selected device."
    //     Not necessary, but the spec says so
    m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

    // Status register:
    //  "BSY shall be cleared to zero indicating command completion."
    //     Already handled by the caller

    //  "DRDY shall be set to one."
    m_regs.status |= StReady;

    //  "DF (Device Fault) shall be cleared to zero."
    //  "DRQ shall be cleared to zero."
    //  "ERR shall be cleared to zero."
    m_regs.status &= ~(StDeviceFault | StDataRequest | StError);

    return true;
}

bool ATADevice::InitializeDeviceParameters() {
    if (m_driver->SetDeviceParameters(m_regs.deviceHead & 0b1111, m_regs.sectorCount)) {
        // Handle normal output as specified in [8.16.5]
            
        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller

        //  "DF (Device Fault) shall be cleared to zero."
        //  "DRQ shall be cleared to zero."
        //  "ERR shall be cleared to zero."
        m_regs.status &= ~(StDeviceFault | StDataRequest | StError);
    }
    else {
        // Handle normal output as specified in [8.16.6]

        // Error register:
        //  "ABRT shall be set to one if the device does not support the requested CHS translation."
        //  "ABRT may be set to one if the device is not able to complete the action requested by the command."
        m_regs.error |= ErrAbort;

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller

        //  "DF (Device Fault) shall be set to one if a device fault has occurred."
        //    TODO: what constitutes a device fault in this case?

        //  "DRQ shall be cleared to zero."
        m_regs.status &= ~StDataRequest;

        //  "ERR shall be set to one if an Error register bit is set to one."
        if (m_regs.error) {
            m_regs.status |= StError;
        }
    }

    return true;
}

bool ATADevice::BeginSecurityUnlock() {
    // [8.34.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    // Sanity check: don't start a PIO transfer while another transfer is running
    if (m_transferActive) {
        return false;
    }

    // Make device ready to accept transfer
    m_transferActive = true;
    m_transferError = false;
    m_dataBufferPos = 0;
    m_sectorsRemaining = 1;
    m_transferHasCommand = true;
    m_command = CmdSecurityUnlock;

    return true;
}

bool ATADevice::SetFeatures() {
    // [8.37.4] The feature to be set is specified in the Features register
    auto feature = m_regs.features;

    bool succeeded;
    switch (feature) {
    case SFCmdSetTransferMode:
        succeeded = SetTransferMode();
        break;
    default:
        log_warning("ATADevice::SetFeatures:  Unimplemented or unsupported feature %d for channel %d, device %d\n", feature, m_channel, m_devIndex);
        succeeded = false;
        break;
    }

    // Handle error as specified in [8.37.6]
    if (!succeeded) {
        // Error register:
        //  "ABRT shall be set to one if this subcommand is not supported or if value is invalid. ABRT may be set
        //   to one if the device is not able to complete the action requested by the command."
        //     Handled by each subcommand

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //     Already handled by the caller

        //  "DRDY shall be set to one."
        m_regs.status |= StReady;

        //  "DRQ shall be cleared to zero."
        m_regs.status &= ~StDataRequest;

        //  "ERR shall be set to one if an Error register bit is set to one."
        if (m_regs.error) {
            m_regs.status |= StError;
        }

    }

    return succeeded;
}

bool ATADevice::SetTransferMode() {
    // [8.37.10] The transfer mode value is specified in the Sector Count register.
    // Parse according to [8.37.10 table 20].
    uint8_t transferType = (m_regs.sectorCount >> 3) & 0b11111;
    uint8_t transferMode = m_regs.sectorCount & 0b111;

    switch (transferType) {
    case XferTypePIODefault:
    case XferTypePIOFlowCtl:
        return SetPIOTransferMode((PIOTransferType)transferType, transferMode);
    case XferTypeMultiWordDMA:
    case XferTypeUltraDMA:
        return SetDMATransferMode((DMATransferType)transferType, transferMode);
    default:
        log_debug("ATADevice::SetTransferMode:  Invalid transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);
        return false;
    }
}

bool ATADevice::SetPIOTransferMode(PIOTransferType type, uint8_t mode) {
    if (type == XferTypePIODefault) {
        if (mode > 1) {
            log_debug("ATADevice::SetPIOTransferMode:  Invalid PIO default transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);
            
            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;
            
            return false;
        }

        log_debug("ATADevice::SetPIOTransferMode:  Setting PIO default transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);

    }
    else {  // XferTypePIOFlowCtl
        if (mode > kMaximumPIOTransferMode) {
            log_debug("ATADevice::SetPIOTransferMode:  Invalid PIO flow control transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("ATADevice::SetPIOTransferMode:  Setting PIO flow control transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);
    }

    m_pioTransferType = type;
    m_pioTransferMode = mode;
    return true;
}

bool ATADevice::SetDMATransferMode(DMATransferType type, uint8_t mode) {
    if (type == XferTypeMultiWordDMA) {
        if (mode > kMaximumMultiwordDMATransferMode) {
            log_debug("ATADevice::SetDMATransferMode:  Invalid Multiword DMA transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("ATADevice::SetDMATransferMode:  Setting Multiword DMA transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);

    }
    else {  // XferTypeUltraDMA
        if (mode > kMaximumUltraDMATransferMode) {
            log_debug("ATADevice::SetDMATransferMode:  Invalid Ultra DMA transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("ATADevice::SetDMATransferMode:  Setting Ultra DMA transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);
    }

    m_dmaTransferType = type;
    m_dmaTransferMode = mode;
    return true;
}

void ATADevice::ExecuteCommand() {
    if (!m_transferHasCommand) {
        return;
    }

    switch (m_command) {
    case CmdSecurityUnlock:
        if (!m_driver->SecurityUnlock(m_dataBuffer)) {
            m_transferError = true;
        }
        break;
    default:
        log_warning("ATADevice::ExecuteCommand: Unknown command 0x%x\n", m_command);
        m_transferError = true;
        break;
    }
}

void ATADevice::WriteSignature(bool packetFeatureSet) {
    // Write signature according to Signature and Persistence protocol [9.1]
    if (packetFeatureSet) {
        m_regs.sectorCount = 0x01;
        m_regs.sectorNumber = 0x01;
        m_regs.cylinder = 0xEB14;
        m_regs.deviceHead = 0x10;
    }
    else {
        m_regs.sectorCount = 0x01;
        m_regs.sectorNumber = 0x01;
        m_regs.cylinder = 0x0000;
        m_regs.deviceHead = 0x00;
    }
}

}
}
}
