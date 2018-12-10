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
#include "proto_dma.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

DMAProtocolCommand::DMAProtocolCommand(ATADevice& device, bool isWrite)
    : IATACommand(device)
    , m_isWrite(isWrite) {
}

DMAProtocolCommand::~DMAProtocolCommand() {
}

void DMAProtocolCommand::Execute() {
    // [8.23.2] Prohibited for devices that support the PACKET Command feature set
    if (m_driver->SupportsPacketCommands()) {
        m_regs.error |= ErrAbort;
        m_regs.status |= StError;
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
        Finish();
        return;
    }

    // [8.23.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        m_regs.error |= ErrAbort;
        m_regs.status |= StError;
        m_regs.status &= ~StBusy;
        m_interrupt.Assert();
        Finish();
        return;
    }

    // Determine if we're using LBA address or CHS numbers
    bool useLBA = (m_regs.deviceHead & DevHeadDMALBA) != 0;

    uint32_t startingLBA;
    uint32_t endingLBA;
    // Read address accordingly and validate parameters
    if (useLBA) {
        startingLBA = ((m_regs.deviceHead & 0b1111) << 24) | (m_regs.cylinder << 8) | (m_regs.sectorNumber);
    }
    else {
        // Convert from CHS to LBA
        uint16_t cylinder = m_regs.cylinder;
        uint8_t head = m_regs.deviceHead & 0b1111;
        uint8_t sector = m_regs.sectorNumber;
        startingLBA = m_driver->CHSToLBA(cylinder, head, sector);
    }

    // Calculate ending LBA
    if (m_regs.sectorCount == 0) {
        endingLBA = startingLBA + 256;
    }
    else {
        endingLBA = startingLBA + m_regs.sectorCount;
    }

    // Check that the address is user accessible
    if (!m_driver->IsLBAAddressUserAccessible(startingLBA)) {
        m_regs.status |= StError;
        m_regs.status &= ~(StBusy | StDataRequest);
        m_interrupt.Assert();
        Finish();
        return;
    }

    // Good to go
    m_startingByte = (uint64_t)startingLBA * kSectorSize;
    m_endingByte = (uint64_t)endingLBA * kSectorSize;
    m_currentByte = m_startingByte;
    m_regs.status &= ~StBusy;
    m_regs.status |= StDataRequest;
}

void DMAProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    // Sanity check: cannot read during a write transfer
    if (m_isWrite) {
        log_warning("DMAProtocolCommand::ReadData:  Trying to read during a DMA write operation\n");
        m_regs.status |= StError;
        Finish();
        return;
    }

    // If the device uses removable media, check if it has media
    // TODO: implement removable media functions in the driver
    /*if (m_driver->IsRemovableMedia() && !m_driver->HasMedium()) {
        // [8.23.6]: "NM shall be set to one if no media is present in a removable media device."
        m_regs.error |= ErrDMANoMedia;
        UnrecoverableError();
        return;
    }*/

    // Check that the removable media was not changed while the operation is in progress
    // TODO: implement removable media functions in the driver
    /*if (m_driver->MediaChangeRequested()) {
        // [8.23.6]: "MCR shall be set to one if a media change request has been detected by a removable media device."
        m_regs.error |= ErrDMAMediaChangeRequest;
        UnrecoverableError();
        return;
    }*/

    // Check that the next sector is accessible
    if (!m_driver->IsLBAAddressUserAccessible(m_currentByte / kSectorSize)) {
        // [8.23.6]: "IDNF shall be set to one if a user-accessible address could not be found"
        m_regs.error |= ErrDMADataNotFound;
        UnrecoverableError();
        return;
    }

    // Try to read the next sector
    if (!m_driver->Read(m_currentByte, value, size)) {
        m_regs.status |= StDeviceFault;
        UnrecoverableError();
        return;
    }

    // Update position
    m_currentByte += size;

    // Check if the DMA transfer has finished
    if (m_currentByte >= m_endingByte) {
        FinishTransfer();
        m_regs.status &= ~(StBusy | StDataRequest);
        m_interrupt.Assert();
    }
}

void DMAProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Sanity check: cannot write during a read transfer
    if (!m_isWrite) {
        log_warning("DMAProtocolCommand::WriteData:  Trying to write during a DMA read operation\n");
        return;
    }

    // If the device uses removable media, check if it has media
    // TODO: implement removable media functions in the driver
    /*if (m_driver->IsRemovableMedia() && !m_driver->HasMedium()) {
        // [8.23.6]: "NM shall be set to one if no media is present in a removable media device."
        m_regs.error |= ErrDMANoMedia;
        UnrecoverableError();
        return;
    }*/

    // Check that the removable media was not changed while the operation is in progress
    // TODO: implement removable media functions in the driver
    /*if (m_driver->MediaChangeRequested()) {
        // [8.23.6]: "MCR shall be set to one if a media change request has been detected by a removable media device."
        m_regs.error |= ErrDMAMediaChangeRequest;
        UnrecoverableError();
        return;
    }*/

    // Check that the next sector is accessible
    if (!m_driver->IsLBAAddressUserAccessible(m_currentByte / kSectorSize)) {
        // [8.23.6]: "IDNF shall be set to one if a user-accessible address could not be found"
        m_regs.error |= ErrDMADataNotFound;
        UnrecoverableError();
        return;
    }

    // Try to write next sector
    if (!m_driver->Write(m_currentByte, value, size)) {
        m_regs.status |= StDeviceFault;
        UnrecoverableError();
        return;
    }

    // Update position
    m_currentByte += size;

    // Check if the DMA transfer has finished
    if (m_currentByte >= m_endingByte) {
        FinishTransfer();
        m_regs.status &= ~(StBusy | StDataRequest);
        m_interrupt.Assert();
    }
}

void DMAProtocolCommand::FinishTransfer() {
    // Handle normal output as specified in [8.23.5]

    // Device/Head register:
    //  "DEV shall indicate the selected device."
    //     Not necessary, but the spec says so
    m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

    // Status register:
    //  "BSY shall be cleared to zero indicating command completion."
    m_regs.status &= ~StBusy;

    //  "DRDY shall be set to one."
    m_regs.status |= StReady;

    //  "DF(Device Fault) shall be cleared to zero."
    //  "DRQ shall be cleared to zero."
    //  "ERR shall be cleared to zero."
    m_regs.status &= ~(StDeviceFault | StDataRequest | StError);

    Finish();
}

void DMAProtocolCommand::UnrecoverableError() {
    // Handle error output as specified in [8.23.6]

    // Error register is handled by ReadData/WriteData

    // Sector Number, Cylinder Low, Cylinder High, Device/Head:
    //  "shall be written with the address of first unrecoverable error."
    m_driver->LBAToCHS(m_currentByte / kSectorSize, &m_regs.cylinder, &m_regs.sectorNumber, &m_regs.deviceHead);

    // Device/Head register:
    //  "DEV shall indicate the selected device."
    //     Not necessary, but the spec says so
    m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

    // Status register:
    //  "BSY shall be cleared to zero indicating command completion."
    m_regs.status &= ~StBusy;

    //  "DRDY shall be set to one."
    m_regs.status |= StReady;

    //  "DF (Device Fault) shall be set to one if a device fault has occurred."
    //    Handled by ReadData/WriteData

    //  "DRQ shall be cleared to zero."
    m_regs.status &= ~StDataRequest;

    //  "ERR shall be set to one if an Error register bit is set to one."
    if (m_regs.error) {
        m_regs.status |= StError;
    }
}

}
}
}
}
