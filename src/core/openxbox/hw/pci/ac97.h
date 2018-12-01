#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class AC97Device : public PCIDevice {
public:
    // constructor
    AC97Device(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
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
