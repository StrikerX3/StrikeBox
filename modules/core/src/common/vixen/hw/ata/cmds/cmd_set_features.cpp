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
#include "vixen/hw/ata/cmds/cmd_set_features.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

SetFeatures::SetFeatures(ATADevice& device)
    : NonDataProtocolCommand(device) {
}

SetFeatures::~SetFeatures() {
}

bool SetFeatures::ExecuteImpl() {
    // [8.37.4] The feature to be set is specified in the Features register
    auto feature = m_regs.features;

    bool succeeded;
    switch (feature) {
    case SFCmdSetTransferMode:
        succeeded = SetTransferMode();
        break;
    default:
        log_warning("SetFeatures::ExecuteImpl:  Unimplemented or unsupported feature %d for channel %d, device %d\n", feature, m_channel, m_devIndex);
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
        m_regs.status &= ~StBusy;

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

bool SetFeatures::SetTransferMode() {
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
        log_debug("SetFeatures::SetTransferMode:  Invalid transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);
        return false;
    }
}

bool SetFeatures::SetPIOTransferMode(PIOTransferType type, uint8_t mode) {
    if (type == XferTypePIODefault) {
        if (mode > 1) {
            log_debug("SetFeatures::SetPIOTransferMode:  Invalid PIO default transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("SetFeatures::SetPIOTransferMode:  Setting PIO default transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);

    }
    else {  // XferTypePIOFlowCtl
        if (mode > kMaximumPIOTransferMode) {
            log_debug("SetFeatures::SetPIOTransferMode:  Invalid PIO flow control transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("SetFeatures::SetPIOTransferMode:  Setting PIO flow control transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);
    }

    m_driver->SetPIOTransferMode(type, mode);
    return true;
}

bool SetFeatures::SetDMATransferMode(DMATransferType type, uint8_t mode) {
    if (type == XferTypeMultiWordDMA) {
        if (mode > kMaximumMultiwordDMATransferMode) {
            log_debug("SetFeatures::SetDMATransferMode:  Invalid Multiword DMA transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("SetFeatures::SetDMATransferMode:  Setting Multiword DMA transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);

    }
    else {  // XferTypeUltraDMA
        if (mode > kMaximumUltraDMATransferMode) {
            log_debug("SetFeatures::SetDMATransferMode:  Invalid Ultra DMA transfer mode specified for channel %d, device %d\n", m_channel, m_devIndex);

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            m_regs.error |= ErrAbort;

            return false;
        }

        log_debug("SetFeatures::SetDMATransferMode:  Setting Ultra DMA transfer mode %d for channel %d, device %d\n", mode, m_channel, m_devIndex);
    }

    m_driver->SetDMATransferMode(type, mode);
    return true;
}

}
}
}
}
