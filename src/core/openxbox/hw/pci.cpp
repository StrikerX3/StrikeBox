#include "pci.h"
#include "openxbox/log.h"

namespace openxbox {

PCIDevice::PCIDevice() {
    for (uint8_t i = 0; i < PCI_NUM_BARS; i++) {
        m_BARs[i].index = i;
        m_BARs[i].size = 0;
        m_BARs[i].reg.value = 0;
    }
}

bool PCIDevice::GetIOBar(uint32_t port, PCIBar* bar) {
    for (uint8_t i = 0; i < PCI_NUM_BARS; i++) {
        if (m_BARs[i].reg.Raw.type == PCI_BAR_TYPE_IO &&
            ((port >> 2) >= m_BARs[i].reg.IO.address) &&
            ((port >> 2) < m_BARs[i].reg.IO.address + m_BARs[i].size)
        ) {
            *bar = m_BARs[i];
            return true;
        }
    }

    return false;
}

bool PCIDevice::GetMMIOBar(uint32_t addr, PCIBar* bar) {
    for (uint8_t i = 0; i < PCI_NUM_BARS; i++) {
        if (m_BARs[i].reg.Raw.type == PCI_BAR_TYPE_MEMORY &&
            ((addr >> 4) >= (m_BARs[i].reg.Memory.address << 4)) &&
            ((addr >> 4) < (m_BARs[i].reg.Memory.address << 4) + m_BARs[i].size)
        ) {
            *bar = m_BARs[i];
            return true;
        }
    }

    return false;
}

bool PCIDevice::RegisterBAR(int index, uint32_t size, uint32_t value) {
    if (m_BARs[index].size != 0) {
        log_warning("PCIDevice::RegisterBar: Trying to register a BAR that is already allocated (index: %d)\n", index);
        return false;

    }

    m_BARs[index].size = size;
    m_BARs[index].reg.value = value;

    return true;
}

bool PCIDevice::UpdateBAR(int index, uint32_t newValue) {
    if (m_BARs[index].size == 0) {
        log_warning("PCIDevice::UpdateBAR: Trying to update a BAR that does not exist (index: %d, value 0x%08X)\n", index, newValue);
        return false;
    }

    m_BARs[index].reg.value = newValue;

    return true;
}

uint32_t PCIDevice::ReadConfigRegister(uint32_t reg) {
    switch (reg) {
    case PCI_CONFIG_DEVICE:
        return (m_deviceID << 16) | m_vendorID;
    case PCI_CONFIG_BAR_0:
    case PCI_CONFIG_BAR_1:
    case PCI_CONFIG_BAR_2:
    case PCI_CONFIG_BAR_3:
    case PCI_CONFIG_BAR_4:
    case PCI_CONFIG_BAR_5:
    {
        int barIndex = (reg - PCI_CONFIG_BAR_0) / 4;
        if (m_BARs[barIndex].size == 0) {
            log_warning("PCIDevice::ReadConfigRegister: Trying to Read a BAR that does not exist (index: %d)\n", barIndex);
            return 0xFFFFFFFF;
        }

        return m_BARs[barIndex].reg.value;
    }
    default:
        log_warning("PCIDevice::ReadConfigRegister:  Unhandled Register %X\n", reg);
        break;
    }

    return 0;
}

void PCIDevice::WriteConfigRegister(uint32_t reg, uint32_t value) {
    switch (reg) {
    case PCI_CONFIG_BAR_0:
    case PCI_CONFIG_BAR_1:
    case PCI_CONFIG_BAR_2:
    case PCI_CONFIG_BAR_3:
    case PCI_CONFIG_BAR_4:
    case PCI_CONFIG_BAR_5:
    {
        int barIndex = (reg - PCI_CONFIG_BAR_0) / 4;
        UpdateBAR(barIndex, value);
        break;
    }
    default:
        log_warning("PCIDevice::WriteConfigRegister: Unhandled Register %X\n", reg);
        break;
    }
}

}
