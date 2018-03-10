#include "pci.h"
#include "openxbox/log.h"

namespace openxbox {

PCIDevice::PCIDevice() {
    memset(m_configSpace, 0, sizeof(m_configSpace));
    memset(m_BARSizes, 0, sizeof(m_BARSizes));
}

bool PCIDevice::GetIOBar(uint32_t port, uint8_t* barIndex, uint32_t *baseAddress) {
    uint8_t headerType = m_configSpace[PCI_CONFIG_HEADER_TYPE];
    uint8_t numBARs;
    uint8_t baseReg;

    switch (headerType) {
    case PCI_TYPE_DEVICE: {
        numBARs = PCI_NUM_BARS_DEVICE;
        baseReg = PCIDEV_CONFIG_BAR_0;
        break;
    }
    case PCI_TYPE_PCI_BRIDGE: {
        numBARs = PCI_NUM_BARS_PCI_BRIDGE;
        baseReg = PCIBRIDGE_CONFIG_BAR_0;
        break;
    }
    default: {
        log_warning("PCIDevice::GetIOBar: invalid device type 0x%x\n", headerType);
        return false;
    }
    }

    for (uint8_t i = 0; i < numBARs; i++) {
        if (m_BARSizes[i] == 0) {
            continue;
        }
        
        PCIBarRegister *bar = reinterpret_cast<PCIBarRegister *>(&m_configSpace[i * sizeof(PCIBarRegister) + baseReg]);
        if (bar->Raw.type == PCI_BAR_TYPE_IO && ((port >> 2) >= bar->IO.address) && ((port >> 2) < bar->IO.address + m_BARSizes[i])) {
            *barIndex = i;
            *baseAddress = bar->IO.address;
            return true;
        }
    }

    return false;
}

bool PCIDevice::GetMMIOBar(uint32_t addr, uint8_t* barIndex, uint32_t *baseAddress) {
    uint8_t headerType = m_configSpace[PCI_CONFIG_HEADER_TYPE];
    uint8_t numBARs;
    uint8_t baseReg;

    switch (headerType) {
    case PCI_TYPE_DEVICE: {
        numBARs = PCI_NUM_BARS_DEVICE;
        baseReg = PCIDEV_CONFIG_BAR_0;
        break;
    }
    case PCI_TYPE_PCI_BRIDGE: {
        numBARs = PCI_NUM_BARS_PCI_BRIDGE;
        baseReg = PCIBRIDGE_CONFIG_BAR_0;
        break;
    }
    default: {
        log_warning("PCIDevice::GetIOBar: invalid device type 0x%x\n", headerType);
        return false;
    }
    }

    for (uint8_t i = 0; i < numBARs; i++) {
        if (m_BARSizes[i] == 0) {
            continue;
        }
        
        PCIBarRegister *bar = reinterpret_cast<PCIBarRegister *>(&m_configSpace[i * sizeof(PCIBarRegister) + baseReg]);
        if (bar->Raw.type == PCI_BAR_TYPE_MEMORY && ((addr >> 4) >= bar->Memory.address) && ((addr >> 4) < bar->Memory.address + m_BARSizes[i])) {
            *barIndex = i;
            *baseAddress = bar->IO.address;
            return true;
        }
    }

    return false;
}

bool PCIDevice::RegisterBAR(int index, uint32_t size) {
    if (m_BARSizes[index] != 0) {
        log_warning("PCIDevice::RegisterBar: Trying to register a BAR that is already allocated (index: %d)\n", index);
        return false;

    }

    m_BARSizes[index] = size;

    return true;
}

void PCIDevice::ReadConfigRegister(uint32_t reg, uint8_t *value, uint8_t size) {
    log_spew("PCIDevice::ReadConfigRegister:  register 0x%x, %d-bit\n", reg, size << 3);
    memcpy(value, &m_configSpace[reg], size);
}

void PCIDevice::WriteConfigRegister(uint32_t reg, uint8_t *value, uint8_t size) {
    log_spew("PCIDevice::WriteConfigRegister: register 0x%x, %d-bit\n", reg, size << 3);
    
    // FIXME: some registers are read-only and/or have side-effects
    memcpy(&m_configSpace[reg], value, size);
}

}
