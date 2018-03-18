#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "../bus/pcibus.h"
#include "pci_irq.h"

namespace openxbox {

class PCIBridgeDevice : public PCIDevice {
public:
    // constructor
    PCIBridgeDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~PCIBridgeDevice();

    // PCI Device functions
    virtual void Init() override;
    virtual void Reset() override;

    void WriteConfig(uint32_t reg, uint32_t value, uint8_t size) override;

    inline PCIBus *GetSecondaryBus() { return m_secBus; }

protected:
    virtual IRQMapper *GetIRQMapper();

private:
    PCIBus *m_secBus;
};

}
