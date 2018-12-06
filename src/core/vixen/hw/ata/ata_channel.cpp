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

ATAChannel::ATAChannel(Channel channel, IRQHandler& irqHandler, uint8_t irqNum)
    : m_channel(channel)
    , m_irqHandler(irqHandler)
    , m_irqNum(irqNum)
{
    for (uint8_t i = 0; i < 2; i++) {
        m_devs[i] = new ATADevice(m_channel, i, m_regs);
    }
}

ATAChannel::~ATAChannel() {
    for (uint8_t i = 0; i < 2; i++) {
        delete m_devs[i];
    }
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
    if ((size & kRegSizes[reg]) == 0) {
        log_debug("ATAChannel::ReadCommandPort: Unexpected read of size %d from register %d for channel %d\n", size, reg, m_channel);
    }

    // Check that there is an attached device
    bool attached = m_devs[m_regs.GetSelectedDeviceIndex()]->IsAttached();

    // If there is no attached device and the guest is attempting to read from
    // a register other than Status, return 0
    if (!attached && reg != RegStatus) {
        *value = 0;
    }
    else {
        switch (reg) {
        case RegData: ReadData(value, size); break;
        case RegError: *value = m_regs.error; break;
        case RegSectorCount: *value = m_regs.sectorCount ; break;
        case RegSectorNumber: *value = m_regs.sectorNumber ; break;
        case RegCylinderLow: *value = m_regs.cylinder & 0xFF ; break;
        case RegCylinderHigh: *value = (m_regs.cylinder >> 8) & 0xFF ; break;
        case RegDeviceHead: *value = m_regs.deviceHead ; break;
        case RegStatus: ReadStatus(reinterpret_cast<uint8_t*>(value)); break;
        }
    }

    return true;
}

bool ATAChannel::WriteCommandPort(Register reg, uint32_t value, uint8_t size) {
    if (reg < RegData || reg > RegCommand) {
        // Should never happen
        log_warning("ATAChannel::WriteCommandPort: Invalid register %d  (channel = %d  size = %d)\n", reg, m_channel, size);
        assert(0);
        return false;
    }

    // Check for operation size mismatch
    if ((size & kRegSizes[reg]) == 0) {
        log_debug("ATAChannel::WriteCommandPort: Unexpected write of size %d to register %d for channel %d\n", size, reg, m_channel);
    }

    // [7.15.6.1] While the device is busy, writes to any command register are ignored,
    // except if sending the Device Reset command
    if (m_regs.status & StBusy) {
        if (reg != RegCommand || value != CmdDeviceReset) {
            log_spew("ATAChannel::WriteCommandPort: Attempted to write register while device is busy\n");
            return true;
        }
    }

    switch (reg) {
    case RegData: WriteData(value, size); break;
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

bool ATAChannel::ReadDMA(uint8_t dstBuffer[kSectorSize]) {
    // Delegate to selected device
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];
    return dev->ReadDMA(dstBuffer);
}

bool ATAChannel::WriteDMA(uint8_t srcBuffer[kSectorSize]) {
    // Delegate to selected device
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];
    return dev->WriteDMA(srcBuffer);
}

bool ATAChannel::IsDMAFinished() {
    // Delegate to selected device
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];
    return dev->IsDMAFinished();
}

bool ATAChannel::EndDMA() {
    // Check if the DMA operation finished successfully
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];

    // Handle result according to DMA command protocol [9.10]
    if (dev->IsDMAFinished()) {
        log_spew("ATAChannel::EndDMA: DMA operation finished successfully on channel %d\n", m_channel);
    }
    else {
        log_spew("ATAChannel::EndDMA: DMA operation failed on channel %d\n", m_channel);

        // Set error status
        m_regs.status |= StError;
    }

    // Tell device that the DMA operation has finished
    dev->EndDMA();

    // Clear BSY=0 and DRQ=0
    m_regs.status &= ~(StBusy | StDataRequest);

    // Assert INTRQ if nIEN=0
    // INTRQ will be negated when the host reads the Status register
    SetInterrupt(true);
    return m_regs.AreInterruptsEnabled();
}

void ATAChannel::ReadData(uint32_t *value, uint8_t size) {
    // Read from device buffer
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];

    // Clear the destination value before reading from the buffer, in case
    // there are not enough bytes to fulfill the request
    *value = 0;
    uint32_t lenRead = dev->ReadBuffer(reinterpret_cast<uint8_t *>(value), size);
    if (lenRead != size) {
        log_warning("ATAChannel::ReadData:  Buffer underflow!  channel = %d  device = %d  size = %d  read = %d\n", m_channel, devIndex, size, lenRead);
    }

    // Update Status register
    if (dev->IsBlockTransferComplete()) {
        if (dev->RequestNextBlock()) {
            // If the device indicates an error, set ERR=1
            // Otherwise, set DRQ=1
            if (dev->HasTransferError()) {
                m_regs.status |= StError;
            }
            else {
                m_regs.status |= StDataRequest;
            }

            // Assert interrupt
            SetInterrupt(true);
        }
        else {
            // Clear DRQ=0
            m_regs.status &= ~StDataRequest;

            dev->FinishCommand();
        }
    }
}

void ATAChannel::ReadStatus(uint8_t *value) {
    // [7.15.4]: "Reading this register when an interrupt is pending causes the interrupt to be cleared."
    SetInterrupt(false);

    *value = m_regs.status;
}

void ATAChannel::WriteData(uint32_t value, uint8_t size) {
    // Write to device buffer
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];

    uint32_t lenWritten = dev->WriteBuffer(reinterpret_cast<uint8_t *>(&value), size);
    if (lenWritten != size) {
        log_warning("ATAChannel::WriteData: Buffer overflow!   channel = %d  device = %d  size = %d  read = %d\n", m_channel, devIndex, size, lenWritten);
    }

    // Update Status register
    if (dev->IsBlockTransferComplete()) {
        // Clear DRQ=0
        m_regs.status &= ~StDataRequest;

        // If the device indicates an error, set ERR=1 clear BSY=0
        // then assert an interrupt
        if (dev->HasTransferError()) {
            m_regs.status |= StError;
        }
        else if (dev->RequestNextBlock()) {
            // Set DRQ=1
            m_regs.status |= StDataRequest;
        }
        else {
            dev->FinishCommand();
        }

        // Assert interrupt
        SetInterrupt(true);
    }
}

void ATAChannel::WriteCommand(uint8_t value) {
    const Command cmd = (Command)value;
    // Check that the command has an protocol associated with it
    if (kCmdProtocols.count(cmd) == 0) {
        log_warning("ATAChannel::WriteCommand: No protocol specified for command 0x%x\n", value);
        m_regs.status |= StError;
        return;
    }

    // Determine which protocol is used by the command and collect all data needed for execution
    auto protocol = kCmdProtocols.at(cmd);
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];
    bool succeeded;

    // Every protocol starts by setting BSY=1
    m_regs.status |= StBusy;
    
    // TODO: submit this entire block as a job to the command processor thread
    {
        switch (cmd) {
        case CmdIdentifyDevice:
            succeeded = dev->IdentifyDevice();
            break;
        case CmdIdentifyPACKETDevice:
            succeeded = dev->IdentifyPACKETDevice();
            break;
        case CmdInitializeDeviceParameters:
            succeeded = dev->InitializeDeviceParameters();
            break;
        case CmdReadDMA:
            succeeded = dev->BeginReadDMA();
            break;
        case CmdSecurityUnlock:
            succeeded = dev->BeginSecurityUnlock();
            break;
        case CmdSetFeatures:
            succeeded = dev->SetFeatures();
            break;
        case CmdWriteDMA:
            succeeded = dev->BeginWriteDMA();
            break;
        default:
            log_warning("ATAChannel::WriteCommand:  Unhandled command 0x%x for channel %d, device %d\n", cmd, m_channel, devIndex);
            succeeded = false;
            break;
        }

        // Assert and negate status flags according to the protocol and the outcome
        if (succeeded) {
            m_regs.status |= protocol.statusAssertedOnSuccess;
        }
        else {
            m_regs.status |= StError;
            m_regs.status &= ~protocol.statusNegatedOnError;
        }

        // Clear BSY=0
        m_regs.status &= ~StBusy;

        // Always assert INTRQ on error, or on success if specified by the protocol
        if (!succeeded || protocol.assertINTRQOnSuccess) {
            // INTRQ is not asserted if nIEN=1
            // INTRQ will be negated when the host reads the Status register
            SetInterrupt(true);
        }

    }
}

void ATAChannel::SetInterrupt(bool asserted) {
    if (asserted != m_interrupt && m_regs.AreInterruptsEnabled()) {
        m_interrupt = asserted;
        m_irqHandler.HandleIRQ(m_irqNum, m_interrupt);
    }
}

}
}
}
