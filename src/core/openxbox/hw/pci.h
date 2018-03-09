#pragma once

#include <cstdint>
#include <map>

namespace openxbox {

#define	PCI_BAR_TYPE_IO       1
#define PCI_BAR_TYPE_MEMORY   0

#define PCI_CONFIG_DEVICE     0x00
#define PCI_CONFIG_BAR_0      0x10
#define PCI_CONFIG_BAR_1      0x14
#define PCI_CONFIG_BAR_2      0x18
#define PCI_CONFIG_BAR_3      0x1C
#define PCI_CONFIG_BAR_4      0x20
#define PCI_CONFIG_BAR_5      0x24

#define PCI_VENDOR_ID_NVIDIA  0x10DE

#define PCI_NUM_BARS  6

class PCIDevice;

typedef struct {

} PCIBarMemory;

typedef struct {

} PCIBarIO;

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

typedef struct {
    uint32_t size;
    uint8_t index;
    PCIBarRegister reg;
} PCIBar;

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
    bool GetIOBar(uint32_t port, PCIBar *bar);
    bool GetMMIOBar(uint32_t addr, PCIBar *bar);
    bool RegisterBAR(int index, uint32_t size, uint32_t defaultValue);
    bool UpdateBAR(int index, uint32_t defaultValue);
    uint32_t ReadConfigRegister(uint32_t reg);
    void WriteConfigRegister(uint32_t reg, uint32_t value);
protected:
    PCIBar m_BARs[PCI_NUM_BARS];
    uint16_t m_deviceID;
    uint16_t m_vendorID;
};

}
