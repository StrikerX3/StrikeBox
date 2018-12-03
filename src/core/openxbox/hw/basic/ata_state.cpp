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

bool ATAState::ReadCommandPort(Register reg, uint32_t *value, uint8_t size) {
    switch (reg) {
    case RegData:
        return ReadData(value, size);
    case RegError:
        return ReadError(value, size);
    case RegSectorCount:
        if (size != 1) { log_spew("ATAState::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, m_channel); }
        *value = m_reg_sectorCount;
        return true;
    case RegSectorNumber:
        if (size != 1) { log_spew("ATAState::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, m_channel); }
        *value = m_reg_sectorNumber;
        return true;
    case RegCylinderLow:
        if (size != 1) { log_spew("ATAState::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, m_channel); }
        *value = m_reg_cylinder & 0xFF;
        return true;
    case RegCylinderHigh:
        if (size != 1) { log_spew("ATAState::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, m_channel); }
        *value = (m_reg_cylinder >> 8) & 0xFF;
        return true;
    case RegDeviceHead:
        if (size != 1) { log_spew("ATAState::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, m_channel); }
        *value = m_reg_deviceHead;
        return true;
    case RegStatus:
        return ReadStatus(value, size);
    default:
        log_warning("ATAState::ReadCommandPort:  Invalid register %d  (channel = %d  size = %d)\n", reg, m_channel, size);
        *value = 0;
        return false;
    }
}

bool ATAState::WriteCommandPort(Register reg, uint32_t value, uint8_t size) {
    switch (reg) {
    case RegData:
        return WriteData(value, size);
    case RegFeatures:
        if (size != 1) { log_spew("ATAState::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, m_channel, value); }
        m_reg_features = value;
        return true;
    case RegSectorCount:
        if (size != 1) { log_spew("ATAState::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, m_channel, value); }
        m_reg_sectorCount = value;
        return true;
    case RegSectorNumber:
        if (size != 1) { log_spew("ATAState::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, m_channel, value); }
        m_reg_sectorNumber = value;
        return true;
    case RegCylinderLow:
        if (size != 1) { log_spew("ATAState::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, m_channel, value); }
        m_reg_cylinder = (m_reg_cylinder & 0xFF00) | (value & 0xFF);
        return true;
    case RegCylinderHigh:
        if (size != 1) { log_spew("ATAState::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, m_channel, value); }
        m_reg_cylinder = (m_reg_cylinder & 0x00FF) | ((value & 0xFF) << 8);
        return true;
    case RegDeviceHead:
        if (size != 1) { log_spew("ATAState::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, m_channel, value); }
        m_reg_deviceHead = value;
        return true;
    case RegCommand:
        return WriteCommand(value, size);
    default:
        log_warning("ATAState::WriteCommandPort:  Invalid register %d  (channel = %d  size = %d  value = 0x%x)\n", reg, m_channel, size, value);
        return false;
    }
}

bool ATAState::ReadControlPort(uint32_t *value, uint8_t size) {
    // Reading from this port returns the contents of the Status register
    return ReadStatus(value, size);
}

bool ATAState::WriteControlPort(uint32_t value, uint8_t size) {
    log_warning("ATAState::WriteControlPort:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", m_channel, size, value);
    return false;
}

bool ATAState::ReadData(uint32_t *value, uint8_t size) {
    if (size != 2) {
        log_warning("ATAState::ReadData:  Unexpected read of size %d  (channel = %d)\n", size, m_channel);
    }
    if (!IsPIOMode()) {
        log_warning("ATAState::ReadData:  Attempted to read while not in PIO mode\n", size, m_channel);
        *value = 0;
        return true;
    }
    log_warning("ATAState::ReadData:  Unimplemented!  (channel = %d  size = %d)\n", m_channel, size);
    *value = 0;
    return false;
}

bool ATAState::ReadError(uint32_t *value, uint8_t size) {
    log_warning("ATAState::ReadError:  Unimplemented!  (channel = %d  size = %d)\n", m_channel, size);
    *value = 0;
    return false;
}

bool ATAState::ReadStatus(uint32_t *value, uint8_t size) {
    log_spew("ATAState::ReadStatus:  Reading status of device %d\n", GetSelectedDeviceIndex());
    //log_warning("ATA::ReadStatus:  Unimplemented!  (channel = %d  size = %d)\n", m_channel, size);
    *value = StReady;
    return false;
}

bool ATAState::WriteData(uint32_t value, uint8_t size) {
    log_warning("ATAState::WriteData:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", m_channel, size, value);
    return false;
}

bool ATAState::WriteCommand(uint32_t value, uint8_t size) {
    log_warning("ATAState::WriteCommand:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", m_channel, size, value);
    return false;
}

}
}
}
