#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class USBPCIDevice : public PCIDevice {
public:
    // constructor
    USBPCIDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~USBPCIDevice();

    // PCI Device functions
    void Init();
    void Reset();

    // TODO: implement I/O
};

}
