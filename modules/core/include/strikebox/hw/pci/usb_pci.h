#pragma once

#include "pci.h"

namespace strikebox {


class USBPCIDevice : public PCIDevice {
public:
    USBPCIDevice(uint8_t irqn, virt86::VirtualProcessor& vp);
    virtual ~USBPCIDevice();

    // PCI Device functions
    void Init();
    void Reset();
    
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;
private:
    uint8_t m_irqn;
    virt86::VirtualProcessor& m_vp;
};

}
