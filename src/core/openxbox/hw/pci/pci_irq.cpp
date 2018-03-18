#include "pci_irq.h"

namespace openxbox {

uint8_t DefaultIRQMapper::MapIRQ(PCIDevice *dev, uint8_t irqNum) {
    return (irqNum + dev->GetPCIAddress().deviceNumber) % PCI_NUM_PINS;
}

bool DefaultIRQMapper::CanSetIRQ() {
    return false;
}

void DefaultIRQMapper::SetIRQ(uint8_t irqNum, int level) {
    // Do nothing
}

}
