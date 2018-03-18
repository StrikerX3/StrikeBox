#pragma once

#include "../basic/irq.h"

namespace openxbox {

#define ISA_NUM_IRQS  16

class ISABus {
public:
    ISABus();
    ~ISABus();

    void ConfigureIRQs(IRQ* irqs);
    IRQ *GetIRQ(uint8_t isaIRQ);
private:
    IRQ *m_irqs;
};

}
