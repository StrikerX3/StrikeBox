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

ATAChannel::ATAChannel(Channel channel, IRQHandler *irqHandler, uint8_t irqNum)
    : m_channel(channel)
    , m_irqHandler(irqHandler)
    , m_irqNum(irqNum)
{
    for (uint8_t i = 0; i < 2; i++) {
        m_devs[i] = new ATADevice(m_channel, i, m_regs);
    }
}

ATAChannel::~ATAChannel() {
    delete[] m_devs;
}

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
    case RegError: *value = m_regs.error; break;
    case RegSectorCount: *value = m_regs.sectorCount; break;
    case RegSectorNumber: *value = m_regs.sectorNumber; break;
    case RegCylinderLow: *value = m_regs.cylinder & 0xFF; break;
    case RegCylinderHigh: *value = (m_regs.cylinder >> 8) & 0xFF; break;
    case RegDeviceHead: *value = m_regs.deviceHead; break;
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
    case RegFeatures: m_regs.features = value; break;
    case RegSectorCount: m_regs.sectorCount = value; break;
    case RegSectorNumber: m_regs.sectorNumber = value; break;
    case RegCylinderLow: m_regs.cylinder = (m_regs.cylinder & 0xFF00) | (value & 0xFF); break;
    case RegCylinderHigh: m_regs.cylinder = (m_regs.cylinder & 0x00FF) | ((value & 0xFF) << 8); break;
    case RegDeviceHead: m_regs.deviceHead = value; break;
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

    m_regs.control = value;

    return false;
}

void ATAChannel::ReadData(uint16_t *value) {
    log_warning("ATAChannel::ReadData:  Unimplemented!  (channel = %d)\n", m_channel);
    *value = 0;
}

void ATAChannel::ReadStatus(uint8_t *value) {
    log_spew("ATAChannel::ReadStatus:  Reading status of device %d\n", GetSelectedDeviceIndex());
    
    *value = m_regs.status | StReady;
    
    // [7.15.4]: "Reading this register when an interrupt is pending causes the interrupt to be cleared."
    SetInterrupt(false);
}

void ATAChannel::WriteData(uint16_t value) {
    log_warning("ATAChannel::WriteData:  Unimplemented!  (channel = %d  value = 0x%04x)\n", m_channel, value);
}

void ATAChannel::WriteCommand(uint8_t value) {
    // Every protocol starts by setting BSY=1
    m_regs.status |= StBusy;

    // TODO: submit this entire block as a job to the command thread
    {
        // TODO: decide which protocol to use
        // Right now the code implements the non-data protocol only [9.9]
        // Needs refactoring to support other protocols
        auto devIndex = GetSelectedDeviceIndex();
        auto dev = m_devs[devIndex];
        bool succeeded;
        switch (value) {
        case CmdSetFeatures:
            succeeded = dev->SetFeatures();
            break;
        case CmdIdentifyDevice:
            succeeded = dev->IdentifyDevice();
            break;
        default:
            log_warning("ATAChannel::WriteCommand:  Unhandled command 0x%x for channel %d, device %d\n", value, m_channel, devIndex);
            succeeded = false;
            break;
        }

        // Error ?
        if (!succeeded) {
            m_regs.status |= StError;
        }

        // Clear BSY=0
        m_regs.status &= ~StBusy;

        // nIEN=0 ?
        if (AreInterruptsEnabled()) {
            // Assert INTRQ
            SetInterrupt(true);
        }
    }
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
