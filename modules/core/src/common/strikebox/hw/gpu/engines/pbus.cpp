// StrikeBox NV2A PBUS (Bus control) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#include "strikebox/hw/gpu/engines/pbus.h"
#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

void PBUS::Reset() {
    m_interruptLevels = 0;
    m_enabledInterrupts = 0;
}

uint32_t PBUS::Read(const uint32_t addr) {
    switch (addr) {
    case Reg_PBUS_INTR: return m_interruptLevels;
    case Reg_PBUS_INTR_ENABLE: return m_enabledInterrupts;
    default:
        if (addr >= 0x800 && addr <= 0x8FF) {
            // PCI Configuration Space access
            // [https://envytools.readthedocs.io/en/latest/hw/bus/pci.html#pbus-mmio-pci]
            return m_nv2a.readPCIConfig(addr - 0x800);
        }

        log_spew("[NV2A] PBUS::Read:   Unimplemented read!   address = 0x%x\n", addr);
        return 0;
    }
}

void PBUS::Write(const uint32_t addr, const uint32_t value) {
    switch (addr) {
    case Reg_PBUS_INTR:
        // Clear specified interrupts
        m_interruptLevels &= ~value;
        m_nv2a.UpdateIRQ();
        break;
    case Reg_PBUS_INTR_ENABLE:
        m_enabledInterrupts = value;
        m_nv2a.UpdateIRQ();
        break;
    default:
        if (addr >= 0x800 && addr <= 0x8FF) {
            // PCI Configuration Space access
            // [https://envytools.readthedocs.io/en/latest/hw/bus/pci.html#pbus-mmio-pci]
            m_nv2a.writePCIConfig(addr - 0x800, value);
        }
        else {
            log_spew("[NV2A] PBUS::Write:  Unimplemented write!   address = 0x%x,  value = 0x%x\n", addr, value);
        }
        break;
    }
}

}
