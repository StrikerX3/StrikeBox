#pragma once

#include "strikebox/io.h"

#include <cstdint>
#include <chrono>

namespace strikebox {

#define CMOS_IRQ   8

#define PORT_CMOS_BASE         0x70
#define PORT_CMOS_COUNT        3

#define PORT_CMOS_CONTROL      0
#define PORT_CMOS_DATA         1
#define PORT_CMOS_EXT_CONTROL  2

enum CMOSRegister {
    RTCSeconds = 0x0,
    RTCSecondsAlarm = 0x1,
    RTCMinutes = 0x2,
    RTCMinutesAlarm = 0x3,
    RTCHours = 0x4,
    RTCHoursAlarm = 0x5,

    RTCDayOfWeek = 0x6,
    RTCDayOfMonth = 0x7,
    RTCMonth = 0x8,
    RTCYear = 0x9,
    RTCCentury = 0x7f,  // Xbox only

    RegA = 0xa,
    RegB = 0xb,
    RegC = 0xc,
    RegD = 0xd,
};

enum CMOSRegisterBit {
    RegA_UIP = (1 << 7),   // (UIP) Update in progress

    RegB_SET = (1 << 7),   // (SET)
    RegB_PIE = (1 << 6),   // (PIE) Periodic interrupt enable
    RegB_AIE = (1 << 5),   // (AIE) Alarm interrupt enable
    RegB_UIE = (1 << 4),   // (UIE) Update-ended interrupt enable
    RegB_SQWE = (1 << 3),  // (SQWE) Square wave enable
    RegB_DM = (1 << 2),    // (DM) Data mode
    RegB_24_12 = (1 << 1), // (24/12) Format output to: 1 = 24-hour format, 2 = 12-hour
    RegB_DSE = (1 << 0),   // (DSE) Daylight savings enable

    RegC_IRQF = (1 << 7),  // (IRQF) Interrupt request flag
    RegC_PF = (1 << 6),    // (PF) Periodic interrupt flag
    RegC_AF = (1 << 5),    // (AF) Alarm flag
    RegC_UF = (1 << 4),    // (AF) Update-ended interrupt flag
};

class CMOS : public IODevice {
public:
    CMOS();
    virtual ~CMOS();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;
private:
    // Selected register address
    uint8_t m_regAddr;

    // CMOS memory
    uint8_t m_memory[0x100];

    // Offset from host RTC (in seconds), used to virtualize the RTC
    int64_t m_offset;

    void ReadRTC(uint8_t reg, uint8_t *value);
    void WriteRTC(uint8_t reg, uint8_t value);
    
    uint8_t ToBCD(uint8_t value);
    uint8_t FromBCD(uint8_t value);

    uint8_t ToBCDHour(uint8_t hour);
    uint8_t FromBCDHour(uint8_t hour);
};

}
