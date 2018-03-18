#include "isabus.h"
#include "openxbox/log.h"

namespace openxbox {

ISABus::ISABus() {
    m_irqs = nullptr;
}

ISABus::~ISABus() {
}

void ISABus::ConfigureIRQs(IRQ* irqs) {
    m_irqs = irqs;
}

IRQ *ISABus::GetIRQ(uint8_t isaIRQ) {
    if (isaIRQ > 15) {
        log_warning("ISABus::GetIRQ: invalid ISA IRQ %u\n", isaIRQ);
        return nullptr;
    }
    return &m_irqs[isaIRQ];
}

}
