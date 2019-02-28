#pragma once

#include "pci.h"

namespace strikebox {

#define PCI_NUM_PINS 4

class IRQMapper {
public:
    virtual ~IRQMapper() {};
    virtual uint8_t MapIRQ(PCIDevice *dev, uint8_t irqNum) = 0;
    virtual bool CanSetIRQ() = 0;
    virtual void SetIRQ(uint8_t irqNum, int level) = 0;
};

class DefaultIRQMapper : public IRQMapper {
public:
    uint8_t MapIRQ(PCIDevice *dev, uint8_t irqNum) override;
    bool CanSetIRQ() override;
    void SetIRQ(uint8_t irqNum, int level) override;
};

}
