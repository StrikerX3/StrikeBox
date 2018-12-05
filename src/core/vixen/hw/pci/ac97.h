#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace vixen {

class AC97Device : public PCIDevice {
public:
    // constructor
    AC97Device();
    virtual ~AC97Device();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;
};

}
