#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "pci_irq.h"
#include "../basic/irq.h"
#include "../bus/isabus.h"

namespace vixen {

#define XBOX_NUM_INT_IRQS 8
#define XBOX_NUM_PIRQS    4

#define XBOX_NUM_PIC_IRQS 16

#define XBOX_LPC_ACPI_IRQ_ROUT 0x64
#define XBOX_LPC_PIRQ_ROUT     0x68
#define XBOX_LPC_INT_IRQ_ROUT  0x6C

class LPCDevice : public PCIDevice, public IRQHandler {
public:
    // constructor
    LPCDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID, IRQ *irqs, uint8_t *rom, uint8_t *bios, uint32_t biosSize, uint8_t *mcpxROM, bool initMcpxROM);
    virtual ~LPCDevice();

    void HandleIRQ(uint8_t irqNum, bool level) override;
    inline ISABus *GetISABus() { return m_isaBus; }

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;

    void WriteConfig(uint32_t reg, uint32_t value, uint8_t size) override;

private:
    int m_field_pin = 0;
    IRQ *m_irqs;
    ISABus *m_isaBus;

    uint8_t *m_rom;
    uint8_t *m_bios;
    uint32_t m_biosSize;
    uint8_t *m_mcpxROM;
    bool m_initMcpxROM;

    friend class LPCIRQMapper;
};

class LPCIRQMapper : public IRQMapper {
public:
    LPCIRQMapper(LPCDevice *lpc);

    uint8_t MapIRQ(PCIDevice *dev, uint8_t irqNum) override;
    bool CanSetIRQ() override;
    void SetIRQ(uint8_t irqNum, int level) override;
private:
    LPCDevice *m_lpc;
};

}
