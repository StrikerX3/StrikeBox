#include "vixen/hw/basic/cmos.h"

#include "vixen/log.h"
#include "vixen/io.h"

namespace vixen {

static inline bool IsRTCRegister(uint8_t reg) {
    switch (reg) {
    case RTCSeconds:
    case RTCMinutes:
    case RTCHours:
    case RTCDayOfWeek:
    case RTCDayOfMonth:
    case RTCMonth:
    case RTCYear:
    case RTCCentury:
        return true;
    default:
        return false;
    }
}

CMOS::CMOS() {
    // TODO: Are IRQs needed?
    // TODO: Persist memory
    memset(m_memory, 0, sizeof(m_memory));
}

CMOS::~CMOS() {
}

void CMOS::Reset() {
    // Reset registers
    m_memory[RegB] &= ~(RegB_PIE | RegB_AIE | RegB_SQWE);
    m_memory[RegC] &= ~(RegC_UF | RegC_IRQF | RegC_PF | RegC_AF);

    // The Xbox kernel checks that the CMOS user memory has an specific pattern
    for (int i = 0x10; i < 0x70; i++) {
        m_memory[i] = 0x55 << (i & 1);
    }

    for (int i = 0x80; i < 0x100; i++) {
        m_memory[i] = 0x55 << (i & 1);
    }

    // Initialize virtual RTC offset to match host RTC
    m_offset = 0;
}

bool CMOS::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(PORT_CMOS_BASE, PORT_CMOS_COUNT, this)) return false;
    
    return true;
}

bool CMOS::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    if (size != 1) {
        log_warning("CMOS::IORead:  Unexpected read of size %d; truncating to 1 byte", size);
        size = 1;
    }

    switch (port - PORT_CMOS_BASE) {
    case PORT_CMOS_DATA:
        if (IsRTCRegister(m_regAddr)) {
            ReadRTC(m_regAddr, reinterpret_cast<uint8_t *>(value));
        }
        else {
            switch (m_regAddr) {
            case RegC: *value = m_memory[m_regAddr] & 0b11110000; break;
            case RegD: *value = m_memory[m_regAddr] & 0b10000000; break;
            default: *value = m_memory[m_regAddr]; break;
            }
        }
        break;
    default:
        log_warning("CMOS::IORead:  Unexpected read!   port = 0x%x\n", port);
        *value = 0;
        break;
    }
    return true;
}

bool CMOS::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    if (size != 1) {
        log_warning("CMOS::IOWrite:  Unexpected write of size %d; truncating to 1 byte", size);
        size = 1;
    }

    switch (port - PORT_CMOS_BASE) {
    case PORT_CMOS_CONTROL:
        m_regAddr = value & 0x7f;
        break;
    case PORT_CMOS_DATA:
        if (IsRTCRegister(m_regAddr)) {
            WriteRTC(m_regAddr, (uint8_t)value);
        }
        else {
            switch (m_regAddr) {
            case RegA:
            case RegB:
            case RegC:
            case RegD:
                // TODO: Handle these
                break;
            }
            m_memory[m_regAddr] = value;
        }
        break;
    case PORT_CMOS_EXT_CONTROL:
        m_regAddr = value;
        break;
    }
    return true;
}

void CMOS::ReadRTC(uint8_t reg, uint8_t *value) {
    // Get current host RTC and apply offset
    auto now = std::chrono::system_clock::now() + std::chrono::seconds(m_offset);
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local = *localtime(&tt);

    // Return requested field
    switch (reg) {
    case RTCSeconds: *value = ToBCD(local.tm_sec); break;
    case RTCMinutes: *value = ToBCD(local.tm_min); break;
    case RTCHours: *value = ToBCDHour(local.tm_hour); break;
    case RTCDayOfWeek: *value = local.tm_wday + 1; break;
    case RTCDayOfMonth: *value = ToBCD(local.tm_mday); break;
    case RTCMonth: *value = ToBCD(local.tm_mon) + 1; break;
    case RTCYear: *value = ToBCD(local.tm_year % 100); break;
    case RTCCentury: *value = ToBCD((local.tm_year + 1900) / 100); break;
    default: log_warning("CMOS::ReadRTC:  Unexpected register %d\n", reg); *value = 0; break;
    }
}

void CMOS::WriteRTC(uint8_t reg, uint8_t value) {
    // Get current host RTC and apply offset
    auto now = std::chrono::system_clock::now() + std::chrono::seconds(m_offset);
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local = *localtime(&tt);
    tm new_local = local;

    // Update requested field
    switch (reg) {
    case RTCSeconds: new_local.tm_sec = FromBCD(value); break;
    case RTCMinutes: new_local.tm_min = FromBCD(value); break;
    case RTCHours: new_local.tm_hour = FromBCDHour(value); break;
    case RTCDayOfWeek: new_local.tm_wday = FromBCD(value) - 1; break;
    case RTCDayOfMonth: new_local.tm_mday = FromBCD(value); break;
    case RTCMonth: new_local.tm_mon = FromBCD(value) - 1; break;
    case RTCYear: new_local.tm_year = (local.tm_year / 100) * 100 + FromBCD(value); break;
    case RTCCentury: new_local.tm_year = local.tm_year % 100 + (FromBCD(value) * 100 - 1900); break;
    default: log_warning("CMOS::WriteRTC:  Unexpected register %d\n", reg); break;
    }

    // Recalculate time offset
    time_t new_tt = mktime(&new_local);
    m_offset = difftime(tt, new_tt);
}

uint8_t CMOS::ToBCD(uint8_t value) {
    if (m_memory[RegB] & RegB_DM) {
        return value;
    }
    else {
        return ((value / 10) << 4) | (value % 10);
    }
}

uint8_t CMOS::FromBCD(uint8_t value) {
    if (m_memory[RegB] & RegB_DM) {
        return value;
    }
    return ((value >> 4) * 10) + (value & 0xf);
}

uint8_t CMOS::ToBCDHour(uint8_t hour) {
    if (m_memory[RegB] & RegB_24_12) {
        return hour;
    }
    
    // Convert 00 -> 12
    uint8_t newHour = (hour % 12) ? (hour % 12) : 12;
    if (hour >= 12) {
        newHour |= 0x80;
    }
    return newHour;
}

uint8_t CMOS::FromBCDHour(uint8_t hour) {
    if (m_memory[RegB] & RegB_24_12) {
        return hour & 0x7f;
    }

    uint8_t newHour = FromBCD(hour & 0x7f);
    return (hour % 12) + ((hour & 0x80) ? 12 : 0);
}

}
