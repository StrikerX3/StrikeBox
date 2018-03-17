#include "gsi.h"

namespace openxbox {

void GSI::Handle(uint8_t irqNum, int level) {
    if (irqNum < ISA_NUM_IRQS) {
        if (i8259IRQs[irqNum] != nullptr) {
            i8259IRQs[irqNum]->handler->Handle(i8259IRQs[irqNum]->num, level);
        }
    }
    if (ioapicIRQs[irqNum] != nullptr) {
        ioapicIRQs[irqNum]->handler->Handle(ioapicIRQs[irqNum]->num, level);
    }
}

}
