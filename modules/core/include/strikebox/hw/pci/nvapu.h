#pragma once

#include "pci.h"

namespace strikebox {

#define NVNET_ADDR  0xFEF00000 
#define NVNET_SIZE  0x00000400

class NVAPUDevice : public PCIDevice {
public:
    NVAPUDevice();
    virtual ~NVAPUDevice();

    // PCI Device functions
    void Init();
    void Reset();
    
    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;
};

}
