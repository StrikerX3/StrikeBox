#pragma once

#include <cstdint>

#include "../defs.h"
#include "pcibridge.h"

namespace vixen {

class AGPBridgeDevice : public PCIBridgeDevice {
public:
    // constructor
    AGPBridgeDevice();
    virtual ~AGPBridgeDevice();

    // PCI Device functions
    void Init() override;
};

}
