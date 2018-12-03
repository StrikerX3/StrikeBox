#include "ata.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {
namespace hw {
namespace ata {

ATA::ATA(i8259 *pic)
    : m_pic(pic) {
}

ATA::~ATA() {
}

void ATA::Reset() {
}

bool ATA::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(kPrimaryCommandBasePort, kPrimaryCommandPortCount, this)) return false;
    if (!mapper->MapIODevice(kPrimaryControlPort, 1, this)) return false;

    if (!mapper->MapIODevice(kSecondaryCommandBasePort, kSecondaryCommandPortCount, this)) return false;
    if (!mapper->MapIODevice(kSecondaryControlPort, 1, this)) return false;

    return true;
}

bool ATA::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    if (port >= kPrimaryCommandBasePort && port <= kPrimaryCommandLastPort) {
        return ReadCommandPort((Register) (port - kPrimaryCommandBasePort), ChanPrimary, value, size);
    }
    if (port == kPrimaryControlPort) {
        return ReadControlPort(ChanPrimary, value, size);
    }
    if (port >= kSecondaryCommandBasePort && port <= kSecondaryCommandLastPort) {
        return ReadCommandPort((Register)(port - kSecondaryCommandBasePort), ChanSecondary, value, size);
    }
    if (port == kSecondaryControlPort) {
        return ReadControlPort(ChanSecondary, value, size);
    }

    log_warning("ATA::IORead:  Unhandled read!   port = 0x%x,  size = %u\n", port, size);
    *value = 0;
    return false;
}

bool ATA::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    if (port >= kPrimaryCommandBasePort && port <= kPrimaryCommandLastPort) {
        return WriteCommandPort((Register)(port - kPrimaryCommandBasePort), ChanPrimary, value, size);
    }
    if (port == kPrimaryControlPort) {
        return WriteControlPort(ChanPrimary, value, size);
    }
    if (port >= kSecondaryCommandBasePort && port <= kSecondaryCommandLastPort) {
        return WriteCommandPort((Register)(port - kSecondaryCommandBasePort), ChanSecondary, value, size);
    }
    if (port == kSecondaryControlPort) {
        return WriteControlPort(ChanSecondary, value, size);
    }

    log_warning("ATA::IOWrite: Unhandled write!  port = 0x%x,  size = %u,  value = 0x%x\n", port, size, value);
    return false;
}

bool ATA::ReadCommandPort(Register reg, Channel channel, uint32_t *value, uint8_t size) {
    switch (reg) {
    case RegData:
        return ReadData(channel, value, size);
    case RegError:
        return ReadError(channel, value, size);
    case RegSectorCount:
        if (size != 1) { log_spew("ATA::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, channel); }
        *value = m_reg_sectorCount;
        return true;
    case RegSectorNumber:
        if (size != 1) { log_spew("ATA::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, channel); }
        *value = m_reg_sectorNumber;
        return true;
    case RegCylinderLow:
        if (size != 1) { log_spew("ATA::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, channel); }
        *value = m_reg_cylinder & 0xFF;
        return true;
    case RegCylinderHigh:
        if (size != 1) { log_spew("ATA::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, channel); }
        *value = (m_reg_cylinder >> 8) & 0xFF;
        return true;
    case RegDeviceHead:
        if (size != 1) { log_spew("ATA::ReadCommandPort: Unexpected read of size %d from register %d, channel %d\n", size, reg, channel); }
        *value = m_reg_deviceHead;
        return true;
    case RegStatus:
        return ReadStatus(channel, value, size);
    default:
        log_warning("ATA::ReadCommandPort:  Invalid register %d  (channel = %d  size = %d)\n", reg, channel, size);
        *value = 0;
        return false;
    }
}

bool ATA::WriteCommandPort(Register reg, Channel channel, uint32_t value, uint8_t size) {
    switch (reg) {
    case RegData:
        return WriteData(channel, value, size);
    case RegFeatures:
        if (size != 1) { log_spew("ATA::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, channel, value); }
        m_reg_features = value;
        return true;
    case RegSectorCount:
        if (size != 1) { log_spew("ATA::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, channel, value); }
        m_reg_sectorCount = value;
        return true;
    case RegSectorNumber:
        if (size != 1) { log_spew("ATA::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, channel, value); }
        m_reg_sectorNumber = value;
        return true;
    case RegCylinderLow:
        if (size != 1) { log_spew("ATA::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, channel, value); }
        m_reg_cylinder = (m_reg_cylinder & 0xFF00) | (value & 0xFF);
        return true;
    case RegCylinderHigh:
        if (size != 1) { log_spew("ATA::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, channel, value); }
        m_reg_cylinder = (m_reg_cylinder & 0x00FF) | ((value & 0xFF) << 8);
        return true;
    case RegDeviceHead:
        if (size != 1) { log_spew("ATA::WriteCommandPort: Unexpected write of size %d from register %d, channel %d  (value = 0x%x)\n", size, reg, channel, value); }
        m_reg_deviceHead = value;
        return true;
    case RegCommand:
        return WriteCommand(channel, value, size);
    default:
        log_warning("ATA::WriteCommandPort:  Invalid register %d  (channel = %d  size = %d  value = 0x%x)\n", reg, channel, size, value);
        return false;
    }
}

bool ATA::ReadControlPort(Channel channel, uint32_t *value, uint8_t size) {
    // Reading from this port returns the contents of the Status register
    return ReadStatus(channel, value, size);
}

bool ATA::WriteControlPort(Channel channel, uint32_t value, uint8_t size) {
    log_warning("ATA::WriteControlPort:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", channel, size, value);
    return false;
}

bool ATA::ReadData(Channel channel, uint32_t *value, uint8_t size) {
    if (size != 2) {
        log_warning("ATA::ReadData:  Unexpected read of size %d  (channel = %d)\n", size, channel);
    }
    if (!IsPIOMode()) {
        log_warning("ATA::ReadData:  Attempted to read while not in PIO mode\n", size, channel);
        *value = 0;
        return true;
    }
    log_warning("ATA::ReadData:  Unimplemented!  (channel = %d  size = %d)\n", channel, size);
    *value = 0;
    return false;
}

bool ATA::ReadError(Channel channel, uint32_t *value, uint8_t size) {
    log_warning("ATA::ReadError:  Unimplemented!  (channel = %d  size = %d)\n", channel, size);
    *value = 0;
    return false;
}

bool ATA::ReadStatus(Channel channel, uint32_t *value, uint8_t size) {
    log_spew("ATA::ReadStatus:  Reading status of device %d\n", GetSelectedDeviceIndex());
    //log_warning("ATA::ReadStatus:  Unimplemented!  (channel = %d  size = %d)\n", channel, size);
    *value = StReady;
    return false;
}

bool ATA::WriteData(Channel channel, uint32_t value, uint8_t size) {
    log_warning("ATA::WriteData:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", channel, size, value);
    return false;
}

bool ATA::WriteCommand(Channel channel, uint32_t value, uint8_t size) {
    log_warning("ATA::WriteCommand:  Unimplemented!  (channel = %d  size = %d  value = 0x%x)\n", channel, size, value);
    return false;
}

}
}
}
