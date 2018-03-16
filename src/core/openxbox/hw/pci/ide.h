#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class IDEDevice : public PCIDevice {
public:
    // constructor
    IDEDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~IDEDevice();

    // PCI Device functions
    void Init();
    void Reset();

    // TODO: implement I/O
};

}
