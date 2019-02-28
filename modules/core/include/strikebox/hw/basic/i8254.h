#pragma once

#include <cstdint>
#include <thread>

#include "irq.h"
#include "strikebox/io.h"

namespace strikebox {

#define PORT_PIT_DATA_0		0x40
#define PORT_PIT_DATA_1		0x41
#define PORT_PIT_DATA_2		0x42
#define PORT_PIT_COMMAND	0x43

#define PORT_PIT_BASE       PORT_PIT_DATA_0
#define PORT_PIT_COUNT      (PORT_PIT_COMMAND - PORT_PIT_DATA_0 + 1)

class i8254 : public IODevice {
public:
    i8254(IRQHandler& irqHandler, float tickRate = 1000.0f);
    virtual ~i8254();
    void Reset();
    
    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;
    
    void Run();
private:
    IRQHandler& m_irqHandler;
    float m_tickRate;
    bool m_running;

    std::thread m_timerThread;
};

}
