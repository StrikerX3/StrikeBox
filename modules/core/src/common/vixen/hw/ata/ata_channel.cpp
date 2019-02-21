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
#include "vixen/hw/ata/ata.h"

#include "vixen/log.h"
#include "vixen/io.h"

namespace vixen {
namespace hw {
namespace ata {

ATAChannel::ATAChannel(Channel channel, IRQHandler& irqHandler, uint8_t irqNum)
    : m_channel(channel)
    , m_irqHandler(irqHandler)
    , m_irqNum(irqNum)
    , m_intrTrigger(IntrTrigger(*this))
    , m_currentCommand(nullptr)
{
    for (uint8_t i = 0; i < 2; i++) {
        m_devs[i] = new ATADevice(m_channel, i, m_regs, m_intrTrigger);
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
        if (m_currentCommand != nullptr) {
            m_currentCommandMem.Free();
            m_currentCommand = nullptr;
        }
    }

    m_regs.control = value;

    return false;
}

void ATAChannel::ReadData(uint32_t *value, uint8_t size) {
    // Check that there is a command in progress
    if (m_currentCommand == nullptr) {
        auto devIndex = m_regs.GetSelectedDeviceIndex();
        log_warning("ATAChannel::ReadData:  No command in progress!  channel = %d  device = %d  size = %d\n", m_channel, devIndex, size);
        return;
    }

    // Read data for the command and clear it if finished
    m_currentCommand->ReadData((uint8_t*)value, size);
    if (m_currentCommand->IsFinished()) {
        //log_spew("ATAChannel::ReadData:  Finished processing command for channel %d\n", m_channel);
        m_currentCommandMem.Free();
        m_currentCommand = nullptr;
    }
}

void ATAChannel::ReadStatus(uint8_t *value) {
    // [7.15.4]: "Reading this register when an interrupt is pending causes the interrupt to be cleared."
    SetInterrupt(false);

    *value = m_regs.status;
}

void ATAChannel::WriteData(uint32_t value, uint8_t size) {
    // Check that there is a command in progress
    if (m_currentCommand == nullptr) {
        auto devIndex = m_regs.GetSelectedDeviceIndex();
        log_warning("ATAChannel::WriteData:  No command in progress!  channel = %d  device = %d  size = %d\n", m_channel, devIndex, size);
        return;
    }

    // Write data for the command and clear it if finished
    m_currentCommand->WriteData((uint8_t*)&value, size);
    if (m_currentCommand->IsFinished()) {
        //log_spew("ATAChannel::WriteData:  Finished processing command for channel %d\n", m_channel);
        m_currentCommandMem.Free();
        m_currentCommand = nullptr;
    }
}

void ATAChannel::WriteCommand(uint8_t value) {
    const Command cmd = (Command)value;
    auto devIndex = m_regs.GetSelectedDeviceIndex();
    auto dev = m_devs[devIndex];

    // Check that there is no command in progress
    // Use double-checked locking to avoid locking when there is no command (the common case)
    if (m_currentCommand != nullptr) {
        std::lock_guard<std::mutex> lk(m_commandMutex);
        if (m_currentCommand != nullptr) {
            log_warning("ATAChannel::WriteCommand:  Trying to run command while another command is in progress\n", value);
            m_regs.status |= StError;
            SetInterrupt(true);
            return;
        }
    }

    // Check that the command has a factory associated with it
    if (kCmdFactories.count(cmd) == 0) {
        log_warning("ATAChannel::WriteCommand:  Unhandled command 0x%x for channel %d, device %d\n", cmd, m_channel, devIndex);
        m_regs.status |= StError;
        SetInterrupt(true);
        return;
    }

    //log_spew("ATAChannel::WriteCommand:  Processing command 0x%x for channel %d, device %d\n", cmd, m_channel, devIndex);

    // Instantiate the command
    auto factory = kCmdFactories.at(cmd);
    m_currentCommand = factory(m_currentCommandMem, *dev);

    // Every protocol starts by setting BSY=1
    m_regs.status |= StBusy;
    
    // TODO: submit this block as a job to the command processor thread
    {
        m_currentCommand->Execute();
        if (m_currentCommand->IsFinished()) {
            //log_spew("ATAChannel::WriteCommand:  Finished processing command 0x%x for channel %d, device %d\n", cmd, m_channel, devIndex);
            m_currentCommandMem.Free();
            m_currentCommand = nullptr;
        }
    }
}

DMATransferResult ATAChannel::ReadDMA(uint8_t *dstBuffer, uint32_t readLen) {
    // Check that there is a command in progress
    if (m_currentCommand == nullptr) {
        auto devIndex = m_regs.GetSelectedDeviceIndex();
        log_warning("ATAChannel::ReadDMA:  No command in progress!  channel = %d  device = %d\n", m_channel, devIndex);
        return DMATransferError;
    }

    // Read data for the command and clear it if finished
    std::lock_guard<std::mutex> lk(m_commandMutex);
    m_currentCommand->ReadData(dstBuffer, readLen);
    if (m_currentCommand->IsFinished()) {
        //log_spew("ATAChannel::ReadDMA:  Finished processing command for channel %d\n", m_channel);
        m_currentCommandMem.Free();
        m_currentCommand = nullptr;
        return DMATransferEnd;
    }

    return DMATransferOK;
}

DMATransferResult ATAChannel::WriteDMA(uint8_t *srcBuffer, uint32_t writeLen) {
    // Check that there is a command in progress
    if (m_currentCommand == nullptr) {
        auto devIndex = m_regs.GetSelectedDeviceIndex();
        log_warning("ATAChannel::WriteDMA:  No command in progress!  channel = %d  device = %d\n", m_channel, devIndex);
        return DMATransferError;
    }

    // Write data for the command and clear it if finished
    std::lock_guard<std::mutex> lk(m_commandMutex);
    m_currentCommand->WriteData(srcBuffer, writeLen);
    if (m_currentCommand->IsFinished()) {
        //log_spew("ATAChannel::WriteDMA:  Finished processing command for channel %d\n", m_channel);
        m_currentCommandMem.Free();
        m_currentCommand = nullptr;
        return DMATransferEnd;
    }

    return DMATransferOK;
}

void ATAChannel::SetInterrupt(bool asserted) {
    if (asserted != m_interrupt && m_regs.AreInterruptsEnabled()) {
        //log_spew("ATAChannel::SetInterrupt:  %s interrupt for channel %d\n", (asserted ? "asserting" : "negating"), m_channel);
        m_interrupt = asserted;
        for (auto it = m_intrHooks.begin(); it != m_intrHooks.end(); it++) {
            (*it)->OnChange(asserted);
        }
        m_irqHandler.HandleIRQ(m_irqNum, m_interrupt);
    }
}

}
}
}
