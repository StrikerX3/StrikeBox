#pragma once

#include <cstdint>

#include "../defs.h"
#include "pcibridge.h"

namespace vixen {

class AGPBridgeDevice : public PCIBridgeDevice {
public:
    // constructor
    AGPBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~AGPBridgeDevice();

    // PCI Device functions
    void Init() override;
};

}
