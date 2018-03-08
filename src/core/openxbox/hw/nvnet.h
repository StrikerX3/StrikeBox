#pragma once

#include "pci.h"

namespace openxbox {

#define NVNET_ADDR  0xFEF00000 
#define NVNET_SIZE  0x00000400

class NVNetDevice : public PCIDevice {
public:
    // PCI Device functions
    void Init();
    void Reset();
    uint32_t IORead(int barIndex, uint32_t port, unsigned size);
    void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size);
    uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
    void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
};

}
