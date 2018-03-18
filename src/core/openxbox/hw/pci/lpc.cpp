#include "lpc.h"
#include "openxbox/log.h"

#include <cassert>

namespace openxbox {

LPCDevice::LPCDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID, IRQ *irqs)
    : PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
        0x06, 0x01, 0x00, // ISA bridge
        /*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
    , m_irqs(irqs)
{
}

LPCDevice::~LPCDevice() {
}

void LPCDevice::HandleIRQ(uint8_t irqNum, int level) {
    uint32_t routing = Read16(m_configSpace, XBOX_LPC_ACPI_IRQ_ROUT);
    
    int irq = (routing >> (irqNum * 8)) & 0xff;
    if (irq == 0 || irq >= XBOX_NUM_PIC_IRQS) {
        return;
    }

    m_irqs[irq].Handle(level);
}

// PCI Device functions

void LPCDevice::Init() {
    RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0x8000 - 0x80FF
}

void LPCDevice::Reset() {
}

void LPCDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("LPCDevice::PCIIORead:   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    
    if (barIndex != 0) {
        log_spew("LPCDevice::PCIIORead:   Unhandled BAR access: %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
        *value = 0;
        return;
    }

    // TODO
    switch (port) {
    case 0x8008: { // TODO: Move 0x8008 TIMER to a device
        if (size == sizeof(uint32_t)) {
            // This timer counts at 3375000 Hz
            auto t = std::chrono::high_resolution_clock::now();
            *value = static_cast<uint32_t>(t.time_since_epoch().count() * 0.003375000);
            return;
        }
        break;
    }
    case 0x80C0: { // TODO: Move 0x80C0 TV encoder to a device
        if (size == sizeof(uint8_t)) {
            // field pin from tv encoder?
            m_field_pin = (m_field_pin + 1) & 1;
            *value = m_field_pin << 5;
            return;
        }
        break;
    }
    }

    log_warning("LPCDevice::PCIIORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
}

void LPCDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    //log_spew("LPCDevice::PCIIOWrite:  bar = %d,  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
    log_warning("LPCDevice::PCIIOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);
}



LPCIRQMapper::LPCIRQMapper(LPCDevice *lpc)
    : m_lpc(lpc)
{
}

uint8_t LPCIRQMapper::MapIRQ(PCIDevice *dev, uint8_t irqNum) {
    int slot = dev->GetPCIAddress().deviceNumber;
    switch (slot) {
    // Devices on the internal bus
    case 2: return 0; // USB0
    case 3: return 1; // USB1
    case 4: return 2; // NIC
    case 5: return 3; // APU
    case 6: return 4; // ACI
    case 9: return 6; // IDE

    case 30: // AGP bridge -> PIRQC?
        return XBOX_NUM_INT_IRQS + 2;
    default:
        // Don't actually know how this should work
        assert(false);
        return XBOX_NUM_INT_IRQS + ((slot + irqNum) & 3);
    }
}

bool LPCIRQMapper::CanSetIRQ() {
    return true;
}

void LPCIRQMapper::SetIRQ(uint8_t irqNum, int level) {
    int picIRQ = 0;

    if (irqNum < XBOX_NUM_INT_IRQS) {
        // Devices on the internal bus
        uint32_t routing = m_lpc->Read16(m_lpc->m_configSpace, XBOX_LPC_INT_IRQ_ROUT);
        picIRQ = (routing >> (irqNum * 4)) & 0xF;

        if (picIRQ == 0) {
            return;
        }
    }
    else {
        // PIRQs
        irqNum -= XBOX_NUM_INT_IRQS;
        picIRQ = m_lpc->m_configSpace[XBOX_LPC_PIRQ_ROUT + irqNum];
    }

    if (picIRQ >= XBOX_NUM_PIC_IRQS) {
        return;
    }

    IRQ *irq = &m_lpc->m_irqs[picIRQ];
    if (irq->handler != nullptr) {
        irq->handler->HandleIRQ(irq->num, level);
    }
}

}
