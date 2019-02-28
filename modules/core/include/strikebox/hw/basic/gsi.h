#pragma once

#include <cstdint>

#include "irq.h"
#include "ioapic.h"
#include "../bus/isabus.h"

namespace strikebox {

#define GSI_NUM_PINS IOAPIC_NUM_PINS

// Global System Interrupts
struct GSI : public IRQHandler {
    IRQ *i8259IRQs[ISA_NUM_IRQS];
    IRQ *ioapicIRQs[IOAPIC_NUM_PINS];

    void HandleIRQ(uint8_t irqNum, bool level) override;
    virtual ~GSI() {}
};

}
