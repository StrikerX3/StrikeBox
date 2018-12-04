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

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {
namespace hw {
namespace ata {

bool ATAChannel::ReadCommandPort(Register reg, uint32_t *value, uint8_t size) {
    if (reg < RegData || reg > RegCommand) {
        // Should never happen
        log_warning("ATAChannel::ReadCommandPort:  Invalid register %d  (channel = %d  size = %d)\n", reg, m_channel, size);
        *value = 0;
        assert(0);
        return false;
    }

    // Check for operation size mismatch
    if (size != kRegSizes[reg]) {
        log_debug("ATAChannel::ReadCommandPort: Unexpected read of size %d from register %d for channel %d\n", size, reg, m_channel);
    }

    switch (reg) {
    case RegData: ReadData(reinterpret_cast<uint16_t*>(value)); break;
    case RegError: *value = m_reg_error; break;
    case RegSectorCount: *value = m_reg_sectorCount; break;
    case RegSectorNumber: *value = m_reg_sectorNumber; break;
    case RegCylinderLow: *value = m_reg_cylinder & 0xFF; break;
    case RegCylinderHigh: *value = (m_reg_cylinder >> 8) & 0xFF; break;
    case RegDeviceHead: *value = m_reg_deviceHead; break;
    case RegStatus: ReadStatus(reinterpret_cast<uint8_t*>(value)); break;
    }

    return true;
}

bool ATAChannel::WriteCommandPort(Register reg, uint32_t value, uint8_t size) {
    if (reg < RegData || reg > RegCommand) {
        // Should never happen
        log_warning("ATAChannel::ReadCommandPort:  Invalid register %d  (channel = %d  size = %d)\n", reg, m_channel, size);
        assert(0);
        return false;
    }

    // Check for operation size mismatch
    if (size != kRegSizes[reg]) {
        log_debug("ATAChannel::ReadCommandPort: Unexpected read of size %d from register %d for channel %d\n", size, reg, m_channel);
    }

    switch (reg) {
    case RegData: WriteData(value); break;
    case RegFeatures: m_reg_features = value; break;
    case RegSectorCount: m_reg_sectorCount = value; break;
    case RegSectorNumber: m_reg_sectorNumber = value; break;
    case RegCylinderLow: m_reg_cylinder = (m_reg_cylinder & 0xFF00) | (value & 0xFF); break;
    case RegCylinderHigh: m_reg_cylinder = (m_reg_cylinder & 0x00FF) | ((value & 0xFF) << 8); break;
    case RegDeviceHead: m_reg_deviceHead = value; break;
    case RegCommand: WriteCommand(value); break;
    }

    return true;
}

bool ATAChannel::ReadControlPort(uint32_t *value, uint8_t size) {
    // Check for operation size mismatch
    if (size != kRegSizes[RegStatus]) {
        log_debug("ATAChannel::ReadControlPort: Unexpected read of size %d for channel %d\n", size, m_channel);
    }

    // Reading from this port returns the contents of the Status register
    ReadStatus(reinterpret_cast<uint8_t *>(value));
    return true;
}

bool ATAChannel::WriteControlPort(uint32_t value, uint8_t size) {
    // Check for operation size mismatch
    if (size != 1) {
        log_debug("ATAChannel::WriteControlPort: Unexpected write of size %d for channel %d\n", size, m_channel);
    }
    
    // Make sure to update the INTRQ state if nIEN is enabled before updating the register
    // to ensure we send a low state to the IRQ handler
    if (value & DevCtlNegateInterruptEnable) {
        SetInterrupt(false);
    }

    if (value & DevCtlSoftwareReset) {
        log_debug("ATAChannel::WriteControlPort: Software reset triggered on channel %d\n", m_channel);
        // TODO: implement [9.3.1] for device 0 and [9.3.2] for device 1
    }

    m_reg_control = value;

    return false;
}

void ATAChannel::ReadData(uint16_t *value) {
    log_warning("ATAChannel::ReadData:  Unimplemented!  (channel = %d)\n", m_channel);
    *value = 0;
}

void ATAChannel::ReadStatus(uint8_t *value) {
    log_spew("ATAChannel::ReadStatus:  Reading status of device %d\n", GetSelectedDeviceIndex());
    
    *value = m_reg_status | StReady;
    
    // [7.15.4]: "Reading this register when an interrupt is pending causes the interrupt to be cleared."
    SetInterrupt(false);
}

void ATAChannel::WriteData(uint16_t value) {
    log_warning("ATAChannel::WriteData:  Unimplemented!  (channel = %d  value = 0x%04x)\n", m_channel, value);
}

void ATAChannel::WriteCommand(uint8_t value) {
    // Follow the non-data protocol [8.37.3] -> [9.9]
   
    // Set BSY=1 and execute command
    m_reg_status |= StBusy;

    // TODO: submit this entire block as a job to the command thread
    {
        bool succeeded;
        switch (value) {
        case CmdSetFeatures:
            succeeded = SetFeatures();

            // [8.37.6]: "If any subcommand input value is not supported or is invalid, the device shall return command aborted."
            if (!succeeded) {
                m_reg_error |= ErrAbort;
            }
            break;
        default:
            log_warning("ATAChannel::WriteCommand:  Unhandled command %d for channel %d, device %d\n", value, m_channel, GetSelectedDeviceIndex());
            succeeded = false;
            break;
        }

        // Error ?
        if (!succeeded) {
            m_reg_status |= StError;
        }

        // Clear BSY=0
        m_reg_status &= ~StBusy;

        // nIEN=0 ?
        if (AreInterruptsEnabled()) {
            // Assert INTRQ
            SetInterrupt(true);
        }
    }
}

bool ATAChannel::SetFeatures() {
    log_debug("ATAChannel::SetFeatures:  Setting features for channel %d, device %d\n", m_channel, GetSelectedDeviceIndex());
    
    // [8.37.4] The feature to be set is specified in the Features register
    switch (m_reg_features) {
    case SFCmdSetTransferMode:
        return SetTransferMode();
    default:
        log_warning("ATAChannel::SetFeatures:  Unimplemented feature %d for channel %d, device %d\n", m_reg_features, m_channel, GetSelectedDeviceIndex());
        return false;
    }
}

bool ATAChannel::SetTransferMode() {
    // [8.37.10] The transfer mode value is specified in the Sector Count register.
    // Parse according to [8.37.10 table 20].
    uint8_t transferType = (m_reg_sectorCount >> 3) & 0b11111;
    uint8_t transferMode = m_reg_sectorCount & 0b111;
    
    switch (transferType) {
    case XferTypePIODefault:
    case XferTypePIOFlowCtl:
        return SetPIOTransferMode((PIOTransferType)transferType, transferMode);
    case XferTypeMultiWordDMA:
    case XferTypeUltraDMA:
        return SetDMATransferMode((DMATransferType)transferType, transferMode);
    default:
        log_debug("ATAChannel::SetTransferMode:  Invalid transfer mode specified for channel %d, device %d\n", m_channel, GetSelectedDeviceIndex());
        return false;
    }
}

bool ATAChannel::SetPIOTransferMode(PIOTransferType type, uint8_t mode) {
    if (type == XferTypePIODefault) {
        if (mode > 1) {
            log_debug("ATAChannel::SetPIOTransferMode:  Invalid PIO default transfer mode specified for channel %d, device %d\n", m_channel, GetSelectedDeviceIndex());
            return false;
        }

        log_debug("ATAChannel::SetPIOTransferMode:  Setting PIO default transfer mode %d for channel %d, device %d\n", mode, m_channel, GetSelectedDeviceIndex());

    }
    else {  // XferTypePIOFlowCtl
        if (mode > kMaximumPIOTransferMode) {
            log_debug("ATAChannel::SetPIOTransferMode:  Invalid PIO flow control transfer mode specified for channel %d, device %d\n", m_channel, GetSelectedDeviceIndex());
            return false;
        }

        log_debug("ATAChannel::SetPIOTransferMode:  Setting PIO flow control transfer mode %d for channel %d, device %d\n", mode, m_channel, GetSelectedDeviceIndex());
    }

    m_pioTransferType = type;
    m_pioTransferMode = mode;
    return true;
}

bool ATAChannel::SetDMATransferMode(DMATransferType type, uint8_t mode) {
    if (type == XferTypeMultiWordDMA) {
        if (mode > kMaximumDMATransferMode) {
            log_debug("ATAChannel::SetDMATransferMode:  Invalid Multiword DMA transfer mode specified for channel %d, device %d\n", m_channel, GetSelectedDeviceIndex());
            return false;
        }
        
        log_debug("ATAChannel::SetDMATransferMode:  Setting Multiword DMA transfer mode %d for channel %d, device %d\n", mode, m_channel, GetSelectedDeviceIndex());

    }
    else {  // XferTypeUltraDMA
        if (mode > kMaximumDMATransferMode) {
            log_debug("ATAChannel::SetDMATransferMode:  Invalid Ultra DMA transfer mode specified for channel %d, device %d\n", m_channel, GetSelectedDeviceIndex());
            return false;
        }

        log_debug("ATAChannel::SetDMATransferMode:  Setting Ultra DMA transfer mode %d for channel %d, device %d\n", mode, m_channel, GetSelectedDeviceIndex());
    }

    m_dmaTransferType = type;
    m_dmaTransferMode = mode;
    return true;
}

void ATAChannel::SetInterrupt(bool asserted) {
    if (asserted != m_interrupt && AreInterruptsEnabled()) {
        m_interrupt = asserted;
        m_irqHandler->HandleIRQ(m_irqNum, m_interrupt);
    }
}

}
}
}
