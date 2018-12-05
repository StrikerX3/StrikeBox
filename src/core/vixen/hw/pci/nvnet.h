#pragma once

#include "pci.h"

namespace vixen {

#define NVNET_ADDR  0xFEF00000 
#define NVNET_SIZE  0x00000400

class NVNetDevice : public PCIDevice {
public:
    NVNetDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~NVNetDevice();

    // PCI Device functions
    void Init();
    void Reset();
    
    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;
};

}
