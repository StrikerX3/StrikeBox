/*
 * Portions of the code are based on XQEMU's SMSC LPC47M157 (Super I/O) emulation.
 * The original copyright header is included below.
 */
/*
 * QEMU SMSC LPC47M157 (Super I/O)
 *
 * Copyright (c) 2013 espes
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include "lpc.h"
#include "vixen/log.h"
#include "vixen/mem.h"

#include <cassert>

namespace vixen {

LPCDevice::LPCDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID, IRQ *irqs, uint8_t *rom, uint8_t *bios, uint32_t biosSize, uint8_t *mcpxROM, bool initMcpxROM)
    : PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
        0x06, 0x01, 0x00, // ISA bridge
        /*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
    , m_irqs(irqs)
    , m_rom(rom)
    , m_bios(bios)
    , m_biosSize(biosSize)
    , m_mcpxROM(mcpxROM)
    , m_initMcpxROM(initMcpxROM)
{
    m_isaBus = new ISABus(irqs);
}

LPCDevice::~LPCDevice() {
    delete m_isaBus;
}

void LPCDevice::HandleIRQ(uint8_t irqNum, bool level) {
    uint32_t routing = Read32(m_configSpace, XBOX_LPC_ACPI_IRQ_ROUT);
    
    int irq = (routing >> (irqNum * 8)) & 0xff;
    if (irq == 0 || irq >= XBOX_NUM_PIC_IRQS) {
        return;
    }

    m_irqs[irq].Handle(level);
}

// PCI Device functions

void LPCDevice::Init() {
    RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0x8000 - 0x80FF

    Reset();
}

void LPCDevice::Reset() {
    // TODO: move to an MCPX component
    // Load BIOS ROM image
    memcpy(m_rom, m_bios, m_biosSize);

    // Replicate BIOS ROM image across the entire 16 MiB range
    for (uint32_t addr = m_biosSize; addr < MiB(16); addr += m_biosSize) {
        memcpy(m_rom + addr, m_rom, m_biosSize);
    }

    if (m_initMcpxROM) {
        // Overlay MCPX ROM image onto the last 512 bytes
        memcpy(m_rom + XBOX_ROM_AREA_SIZE - 512, m_mcpxROM, 512);
    }
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

void LPCDevice::WriteConfig(uint32_t reg, uint32_t value, uint8_t size) {
    PCIDevice::WriteConfig(reg, value, size);

    // TODO: move to an MCPX component
    // Disable MCPX ROM
    if (reg == 0x80 && (value & 2)) {
        log_debug("LPCDevice::WriteConfig:  Disabling MCPX ROM\n");
        // Restore last 512 bytes of the original BIOS ROM image
        memcpy(m_rom + XBOX_ROM_AREA_SIZE - 512, m_bios + m_biosSize - 512, 512);
    }
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
        uint32_t routing = m_lpc->Read32(m_lpc->m_configSpace, XBOX_LPC_INT_IRQ_ROUT);
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
    irq->Handle(level);
}

}
