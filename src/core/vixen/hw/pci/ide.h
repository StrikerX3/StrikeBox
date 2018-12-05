#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace vixen {

class IDEDevice : public PCIDevice {
public:
    // constructor
    IDEDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~IDEDevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
};

}
