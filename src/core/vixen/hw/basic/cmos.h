#pragma once

#include <cstdint>

#include "vixen/cpu.h"

namespace vixen {

#define PORT_CMOS_CONTROL      0x70
#define PORT_CMOS_DATA         0x71
#define PORT_CMOS_EXT_CONTROL  0x72

#define PORT_CMOS_BASE       PORT_CMOS_CONTROL
#define PORT_CMOS_COUNT      (PORT_CMOS_EXT_CONTROL - PORT_CMOS_CONTROL + 1)

class CMOS : public IODevice {
public:
    CMOS();
    virtual ~CMOS();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;
private:
};

}
