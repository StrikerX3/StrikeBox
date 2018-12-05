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

uint32_t ATADevice::ReadBuffer(uint8_t *dest, uint32_t length) {
    uint32_t lenToRead = length;
    if (m_dataBufferPos + length > kSectorSize) {
        lenToRead = GetRemainingBufferLength();
    }

    memcpy(dest, m_dataBuffer + m_dataBufferPos, lenToRead);
    m_dataBufferPos += lenToRead;
    return lenToRead;
}

uint32_t ATADevice::GetRemainingBufferLength() {
    return kSectorSize - m_dataBufferPos;
}

bool ATADevice::IdentifyDevice() {
    // [8.12.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return false;
    }

    // The documentation is very terse regarding error output for this command.
    // I'll assume this command may never fail.

    // Ask the device driver to identify itself
    m_dataBufferPos = 0;
    m_driver->IdentifyDevice(reinterpret_cast<IdentifyDeviceData *>(m_dataBuffer));

    // Handle normal output as specified in [8.12.5.1]
    // TODO: support PACKET commands and implement [8.12.5.2]
    
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
    m_regs.status &= ~(StBit5 | StDataRequest | StError);

    return true;
}

bool ATADevice::BeginReadDMA() {
    // Sanity check: don't start a DMA transfer while another transfer is running
    if (m_transferActive) {
        return false;
    }

    // Get sector count
    m_dma_sectorCount = m_regs.sectorCount;

    // Determine if we're using LBA address or CHS numbers
    m_dma_useLBA = (m_regs.deviceHead & DevHeadDMALBA) != 0;

    // Read address accordingly and validate parameters
    if (m_dma_useLBA) {
        m_dma_lbaAddress = ((m_regs.deviceHead & 0b1111) << 24) | (m_regs.cylinder << 8) | (m_regs.sectorNumber);
        if (m_driver->IsLBAAddressUserAccessible(m_dma_lbaAddress)) {
            m_transferActive = true;
            return true;
        }
        return false;
    }
    else {
        m_dma_cylinder = m_regs.cylinder;
        m_dma_head = m_regs.deviceHead & 0b1111;
        m_dma_sector = m_regs.sectorNumber;
        if (m_driver->IsCHSAddressUserAccessible(m_dma_cylinder, m_dma_head, m_dma_sector)) {
            m_transferActive = true;
            return true;
        }
        return false;
    }
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

}
}
}
