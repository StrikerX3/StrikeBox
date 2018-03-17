#pragma once

#include <cstdint>

namespace openxbox {

class IRQHandler {
public:
    virtual void Handle(uint8_t irqNum, int level) = 0;
};

struct IRQ {
    IRQHandler *handler;
    uint8_t num;
};

IRQ *AllocateIRQs(IRQHandler *handler, uint8_t numIRQs);

}
