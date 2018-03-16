#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class HostBridgeDevice : public PCIDevice {
public:
    // constructor
    HostBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~HostBridgeDevice();

    // PCI Device functions
    void Init();
    void Reset();
};

}
