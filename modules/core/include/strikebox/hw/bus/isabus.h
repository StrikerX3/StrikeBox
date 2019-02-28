#pragma once

#include "../basic/irq.h"

namespace strikebox {

#define ISA_NUM_IRQS  16

class ISABus {
public:
    ISABus(IRQ *irqs);
    ~ISABus();

    IRQ *GetIRQ(uint8_t isaIRQ);
private:
    IRQ *m_irqs;
};

}
