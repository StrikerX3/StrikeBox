#pragma once

#include "../pci/pci.h"
#include "openxbox/io.h"

#include <map>

namespace openxbox {

#define PORT_PCI_CONFIG_ADDRESS   0xCF8
#define PORT_PCI_CONFIG_DATA      0xCFC
#define PCI_CONFIG_REGISTER_MASK  0xFC

class PCIBus : public IODevice {
public:
    virtual ~PCIBus();
    bool MapIO(IOMapper *mapper);
    
    void ConnectDevice(uint32_t deviceId, PCIDevice *pDevice);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;

    bool MMIORead(uint32_t addr, uint32_t *value, uint8_t size) override;
    bool MMIOWrite(uint32_t addr, uint32_t value, uint8_t size) override;

    void Reset();
private:
    void IOWriteConfigAddress(uint32_t pData);
    void IOWriteConfigData(uint32_t pData, uint8_t size, uint8_t regOffset);
    uint32_t IOReadConfigData(uint8_t size, uint8_t regOffset);

    std::map<uint32_t, PCIDevice*> m_Devices;
    PCIConfigAddressRegister m_configAddressRegister;
};

}
