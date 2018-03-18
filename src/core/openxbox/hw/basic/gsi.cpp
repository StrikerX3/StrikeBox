#include "gsi.h"

namespace openxbox {

void GSI::HandleIRQ(uint8_t irqNum, int level) {
    if (irqNum < ISA_NUM_IRQS) {
        i8259IRQs[irqNum]->Handle(level);
    }
    ioapicIRQs[irqNum]->Handle(level);
}

}
