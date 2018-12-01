#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class NVAPUDevice : public PCIDevice {
public:
    // constructor
    NVAPUDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~NVAPUDevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;
};

}
