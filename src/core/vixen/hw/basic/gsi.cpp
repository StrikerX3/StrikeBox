#include "gsi.h"

namespace vixen {

void GSI::HandleIRQ(uint8_t irqNum, bool level) {
    if (irqNum < ISA_NUM_IRQS) {
        if (i8259IRQs[irqNum] != nullptr) {
            i8259IRQs[irqNum]->Handle(level);
        }
    }
    if (ioapicIRQs[irqNum] != nullptr) {
        ioapicIRQs[irqNum]->Handle(level);
    }
}

}
