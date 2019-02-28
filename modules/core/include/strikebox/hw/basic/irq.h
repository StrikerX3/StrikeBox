#pragma once

#include <cstdint>

namespace strikebox {

class IRQHandler {
public:
    virtual void HandleIRQ(uint8_t irqNum, bool level) = 0;
};

struct IRQ {
    IRQHandler *handler;
    uint8_t num;

    inline void Handle(bool level) {
        if (handler != nullptr) {
            handler->HandleIRQ(num, level);
        }
    }
};

IRQ *AllocateIRQs(IRQHandler *handler, uint8_t numIRQs);

}
