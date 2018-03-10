#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace openxbox {

class PCIBridgeDevice : public PCIDevice {
public:
    // constructor
	PCIBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);

    // PCI Device functions
    virtual void Init() override;
    virtual void Reset() override;

    virtual uint32_t IORead(int barIndex, uint32_t port, unsigned size) override;
    virtual void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) override;
    virtual uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size) override;
    virtual void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) override;

    void WriteConfig(uint32_t reg, uint32_t value, uint8_t size) override;
};

}
