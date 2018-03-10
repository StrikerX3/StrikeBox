#pragma once

#include <cstdint>

namespace openxbox {

#define	PCI_BAR_TYPE_IO          1
#define PCI_BAR_TYPE_MEMORY      0

#define PCI_NUM_BARS_DEVICE      6
#define PCI_NUM_BARS_PCI_BRIDGE  2

#define PCI_TYPE_DEVICE          0x00
#define PCI_TYPE_PCI_BRIDGE      0x01
// PCI-to-CardBus bridge (type 0x02) is not used on Xbox

// Common PCI configuration space fields
#define PCI_CONFIG_VENDOR_ID     0x00
#define PCI_CONFIG_DEVICE_ID     0x02
#define PCI_CONFIG_COMMAND       0x04
#define PCI_CONFIG_STATUS        0x06
#define PCI_CONFIG_HEADER_TYPE   0x0E

// PCI configuration space fields specific to devices
#define PCIDEV_CONFIG_BAR_0      0x10
#define PCIDEV_CONFIG_BAR_1      0x14
#define PCIDEV_CONFIG_BAR_2      0x18
#define PCIDEV_CONFIG_BAR_3      0x1C
#define PCIDEV_CONFIG_BAR_4      0x20
#define PCIDEV_CONFIG_BAR_5      0x24

// PCI configuration space fields specific to PCI bridges
#define PCIBRIDGE_CONFIG_BAR_0   0x10
#define PCIBRIDGE_CONFIG_BAR_1   0x14

#define PCI_VENDOR_ID_NVIDIA     0x10DE

class PCIDevice;

typedef struct {
    union {
        struct {
            uint32_t type : 1;
            uint32_t locatable : 2;
            uint32_t prefetchable : 1;
            uint32_t address : 28;
        } Memory;

        struct {
            uint32_t type : 1;
            uint32_t reserved : 1;
            uint32_t address : 30;
        } IO;

        struct {
            uint32_t type : 1;
            uint32_t other : 31;
        } Raw;

        uint32_t value;
    };

} PCIBarRegister;

class PCIDevice {
    // PCI Device Interface
public:
    virtual void Init() = 0;
    virtual void Reset() = 0;
    virtual uint32_t IORead(int barIndex, uint32_t port, unsigned size) = 0;
    virtual void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) = 0;
    virtual uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size) = 0;
    virtual void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) = 0;

    // PCI Device Implementation
public:
    PCIDevice();
    bool GetIOBar(uint32_t port, uint8_t* barIndex, uint32_t *baseAddress);
    bool GetMMIOBar(uint32_t addr, uint8_t* barIndex, uint32_t *baseAddress);
    bool RegisterBAR(int index, uint32_t size);
    
    void ReadConfigRegister(uint32_t reg, uint8_t *value, uint8_t size);
    void WriteConfigRegister(uint32_t reg, uint8_t *value, uint8_t size);
protected:
    uint32_t m_BARSizes[PCI_NUM_BARS_DEVICE];

    uint8_t m_configSpace[256];

    inline uint8_t  ReadConfigRegister8 (uint32_t reg) { return m_configSpace[reg]; }
    inline uint16_t ReadConfigRegister16(uint32_t reg) { return *reinterpret_cast<uint16_t *>(&m_configSpace[reg]); }
    inline uint32_t ReadConfigRegister32(uint32_t reg) { return *reinterpret_cast<uint32_t *>(&m_configSpace[reg]); }

    inline void WriteConfigRegister8 (uint32_t reg, uint8_t  value) { m_configSpace[reg] = value; }
    inline void WriteConfigRegister16(uint32_t reg, uint16_t value) { *reinterpret_cast<uint16_t *>(&m_configSpace[reg]) = value; }
    inline void WriteConfigRegister32(uint32_t reg, uint32_t value) { *reinterpret_cast<uint32_t *>(&m_configSpace[reg]) = value; }
};

}
