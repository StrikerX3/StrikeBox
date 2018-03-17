#pragma once

#include <cstdint>

#include "irq.h"
#include "ioapic.h"
#include "../bus/isabus.h"

namespace openxbox {

#define GSI_NUM_PINS IOAPIC_NUM_PINS

// Global System Interrupts
struct GSI : public IRQHandler {
    IRQ *i8259IRQs[ISA_NUM_IRQS];
    IRQ *ioapicIRQs[IOAPIC_NUM_PINS];

    void Handle(uint8_t irqNum, int level) override;
};

}
