#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class AC97Device : public PCIDevice {
public:
    // constructor
	AC97Device(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);

    // PCI Device functions
    void Init();
    void Reset();

    // TODO: implement I/O
};

}
