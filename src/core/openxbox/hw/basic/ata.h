#pragma once

#include <cstdint>

#include "openxbox/cpu.h"
#include "../basic/i8259.h"

namespace openxbox {

#define PRIMARY_ATA_IRQ                   14
#define PORT_PRIMARY_ATA_COMMAND_BASE     0x1f0
#define PORT_PRIMARY_ATA_COMMAND_COUNT    8
#define PORT_PRIMARY_ATA_CONTROL          0x3f6

#define SECONDARY_ATA_IRQ                 15
#define PORT_SECONDARY_ATA_COMMAND_BASE   0x170
#define PORT_SECONDARY_ATA_COMMAND_COUNT  8
#define PORT_SECONDARY_ATA_CONTROL        0x376


class ATA : public IODevice {
public:
    ATA(i8259 *pic);
    virtual ~ATA();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;
private:
    i8259 *m_pic;
};

}
