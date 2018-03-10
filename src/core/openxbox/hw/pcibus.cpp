#include "pcibus.h"
#include "openxbox/log.h"

namespace openxbox {

void PCIBus::ConnectDevice(uint32_t deviceId, PCIDevice *pDevice) {
    if (m_Devices.find(deviceId) != m_Devices.end()) {
        log_warning("PCIBus: Attempting to connect two devices to the same device address\n");
        return;
    }

    m_Devices[deviceId] = pDevice;
    pDevice->Init();
}

void PCIBus::IOWriteConfigAddress(uint32_t pData) {
    memcpy(&m_configAddressRegister, &pData, sizeof(PCIConfigAddressRegister));
}

uint32_t PCIBus::IOReadConfigData(uint8_t size) {
    log_spew("PCIBus::IOReadConfigData:  (%d:%d:%d reg 0x%x size %d)\n",
        m_configAddressRegister.busNumber,
        m_configAddressRegister.deviceNumber,
        m_configAddressRegister.functionNumber,
        m_configAddressRegister.registerNumber,
        size
    );

    auto it = m_Devices.find(
        PCI_DEVID(m_configAddressRegister.busNumber,
            PCI_DEVFN(m_configAddressRegister.deviceNumber, m_configAddressRegister.functionNumber)
        )
    );
    if (it != m_Devices.end()) {
        return it->second->ReadConfigRegister(m_configAddressRegister.registerNumber & PCI_CONFIG_REGISTER_MASK, size);
    }

    log_warning("PCIBus::IOReadConfigData:  Invalid Device Read  (%d:%d:%d reg 0x%x size %d)\n",
        m_configAddressRegister.busNumber,
        m_configAddressRegister.deviceNumber,
        m_configAddressRegister.functionNumber,
        m_configAddressRegister.registerNumber,
        size
    );

    // Unpopulated PCI slots return 0xFFFFFFFF
    return 0xFFFFFFFF;
}

void PCIBus::IOWriteConfigData(uint32_t pData, uint8_t size) {
    log_spew("PCIBus::IOWriteConfigData: (%d:%d:%d reg 0x%x size %d) = 0x%x\n",
        m_configAddressRegister.busNumber,
        m_configAddressRegister.deviceNumber,
        m_configAddressRegister.functionNumber,
        m_configAddressRegister.registerNumber,
        size,
        pData
    );

    auto it = m_Devices.find(
        PCI_DEVID(m_configAddressRegister.busNumber,
            PCI_DEVFN(m_configAddressRegister.deviceNumber, m_configAddressRegister.functionNumber)
        )
    );
    if (it != m_Devices.end()) {
        it->second->WriteConfigRegister(m_configAddressRegister.registerNumber & PCI_CONFIG_REGISTER_MASK, pData, size);
        return;
    }

    log_warning("PCIBus::IOWriteConfigData: Invalid Device Write (%d:%d:%d reg 0x%x size %d) = 0x%x\n",
        m_configAddressRegister.busNumber,
        m_configAddressRegister.deviceNumber,
        m_configAddressRegister.functionNumber,
        m_configAddressRegister.registerNumber,
        size,
        pData
    );
}

bool PCIBus::IORead(uint32_t addr, uint32_t* data, unsigned size) {
    switch (addr) {
    case PORT_PCI_CONFIG_DATA: // 0xCFC
        *data = IOReadConfigData(size);
        return true;
    default:
        for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
            PCIBar bar;
            if (it->second->GetIOBar(addr, &bar)) {
                *data = it->second->IORead(bar.index, addr - bar.reg.IO.address, size);
                return true;
            }
        }
    }

    return false;
}

bool PCIBus::IOWrite(uint32_t addr, uint32_t value, unsigned size) {
    switch (addr) {
    case PORT_PCI_CONFIG_ADDRESS: // 0xCF8
        if (size == sizeof(uint32_t)) {
            IOWriteConfigAddress(value);
            return true;
        }
        else {
            log_warning("PCIBus:IOWrite: Writing %d-bit PCI config address,  address 0x%x,  value 0x%x\n", size << 3, addr, value);
            IOWriteConfigAddress(value);
            return true;
        }
        break;
    case PORT_PCI_CONFIG_DATA: // 0xCFC
        IOWriteConfigData(value, size);
        return true; // TODO : Should IOWriteConfigData() success/failure be returned?
    default:
        for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
            PCIBar bar;
            if (it->second->GetIOBar(addr, &bar)) {
                it->second->IOWrite(bar.index, addr - (bar.reg.IO.address << 2), value, size);
                return true;
            }
        }
    }

    return false;
}

bool PCIBus::MMIORead(uint32_t addr, uint32_t* data, unsigned size) {
    for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
        PCIBar bar;
        if (it->second->GetMMIOBar(addr, &bar)) {
            *data = it->second->MMIORead(bar.index, addr - (bar.reg.Memory.address << 4), size);
            return true;
        }
    }

    return false;
}

bool PCIBus::MMIOWrite(uint32_t addr, uint32_t value, unsigned size) {
    for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
        PCIBar bar;
        if (it->second->GetMMIOBar(addr, &bar)) {
            it->second->MMIOWrite(bar.index, addr - (bar.reg.Memory.address << 4), value, size);
            return true;
        }
    }

    return false;
}

void PCIBus::Reset() {
    for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
        it->second->Reset();
    }
}

}
