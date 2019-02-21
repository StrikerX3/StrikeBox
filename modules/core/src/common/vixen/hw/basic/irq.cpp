#include "vixen/hw/basic/irq.h"

namespace vixen {

IRQ *AllocateIRQs(IRQHandler *handler, uint8_t numIRQs) {
    IRQ *irqs = new IRQ[numIRQs];
    for (uint8_t i = 0; i < numIRQs; i++) {
        irqs[i].handler = handler;
        irqs[i].num = i;
    }
    return irqs;
}

}
