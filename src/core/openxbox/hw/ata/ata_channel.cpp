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

    log_warning("ATAChannel::WriteControlPort:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", m_channel, size, value);
    return false;
}

void ATAChannel::ReadData(uint16_t *value) {
    if (!IsPIOMode()) {
        log_warning("ATAChannel::ReadData:  Attempted to read while not in PIO mode\n", m_channel);
        *value = 0;
        return;
    }

    log_warning("ATAChannel::ReadData:  Unimplemented!  (channel = %d)\n", m_channel);
    *value = 0;
}

void ATAChannel::ReadStatus(uint8_t *value) {
    log_spew("ATAChannel::ReadStatus:  Reading status of device %d\n", GetSelectedDeviceIndex());
    
    // TODO: implement
    *value = StReady;
    
    // [7.15.4]: "Reading this register when an interrupt is pending causes the interrupt to be cleared"
    m_pendingInterrupt = false;
}

void ATAChannel::WriteData(uint16_t value) {
    log_warning("ATAChannel::WriteData:  Unimplemented!  (channel = %d  value = 0x%04x)\n", m_channel, value);
}

void ATAChannel::WriteCommand(uint8_t value) {
    log_warning("ATAChannel::WriteCommand:  Unimplemented!  (channel = %d  value = 0x%02x)\n", m_channel, value);
}

}
}
}
