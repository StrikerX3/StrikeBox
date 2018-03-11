#pragma once

#include <cstdint>

#include "i8259.h"
#include "openxbox/io.h"

namespace openxbox {

#define PORT_PIT_DATA_0		0x40
#define PORT_PIT_DATA_1		0x41
#define PORT_PIT_DATA_2		0x42
#define PORT_PIT_COMMAND	0x43

class i8254 : public IODevice {
public:
    i8254(i8259 *pic, float tickRate = 1000.0f);
    void Reset();
    
    void Run();

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;
private:
    i8259 *m_pic;
    float m_tickRate;
    bool m_running;
};

}
