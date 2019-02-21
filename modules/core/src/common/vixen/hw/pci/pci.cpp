/*
 * Portions of the code are based on QEMU's PCI bus manager.
 * The original copyright header is included below.
 */
/*
 * QEMU PCI bus manager
 *
 * Copyright (c) 2004 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "vixen/hw/pci/pci.h"

#include "vixen/hw/bus/pcibus.h"
#include "vixen/log.h"

#include <cassert>

namespace vixen {

PCIDevice::PCIDevice(uint8_t type, uint16_t vendorID, uint16_t deviceID,
    uint8_t revisionID, uint8_t classID, uint8_t subclass, uint8_t progIF,
    uint16_t subsystemVendorID, uint16_t subsystemID
) {
    memset(m_configSpace, 0, sizeof(m_configSpace));
    memset(m_writeMask, 0, sizeof(m_writeMask));
    memset(m_checkMask, 0, sizeof(m_checkMask));
    memset(m_write1ToClearMask, 0, sizeof(m_write1ToClearMask));
    memset(m_BARSizes, 0, sizeof(m_BARSizes));

    m_bus = nullptr;
    m_irqState = 0;

    Write8(m_configSpace, PCI_HEADER_TYPE, type);
    Write16(m_configSpace, PCI_VENDOR_ID, vendorID);
    Write16(m_configSpace, PCI_DEVICE_ID, deviceID);

    Write8(m_configSpace, PCI_REVISION_ID, revisionID);
    Write16(m_configSpace, PCI_CLASS_DEVICE, subclass | (classID << 8));
    Write8(m_configSpace, PCI_CLASS_PROG, progIF);

    if (type == PCI_HEADER_TYPE_BRIDGE) {
        Write16(m_configSpace, PCI_SUBSYSTEM_VENDOR_ID, subsystemVendorID);
        Write16(m_configSpace, PCI_SUBSYSTEM_ID, subsystemID);
    }

    Write16(m_checkMask, PCI_VENDOR_ID, 0xffff);
    Write16(m_checkMask, PCI_DEVICE_ID, 0xffff);
    Write8(m_checkMask, PCI_STATUS, PCI_STATUS_CAP_LIST);
    Write8(m_checkMask, PCI_REVISION_ID, 0xff);
    Write8(m_checkMask, PCI_CLASS_PROG, 0xff);
    Write16(m_checkMask, PCI_CLASS_DEVICE, 0xffff);
    Write8(m_checkMask, PCI_HEADER_TYPE, 0xff);
    Write8(m_checkMask, PCI_CAPABILITY_LIST, 0xff);

    Write8(m_writeMask, PCI_CACHE_LINE_SIZE, 0xff);
    Write8(m_writeMask, PCI_INTERRUPT_LINE, 0xff);
    Write16(m_writeMask, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INTX_DISABLE);
    TestAndSet16(m_writeMask, PCI_COMMAND, PCI_COMMAND_SERR);

    memset(m_writeMask + PCI_CONFIG_HEADER_SIZE, 0xff, 256 - PCI_CONFIG_HEADER_SIZE);

    Write16(m_write1ToClearMask, PCI_STATUS,
        PCI_STATUS_PARITY | PCI_STATUS_SIG_TARGET_ABORT | PCI_STATUS_REC_TARGET_ABORT |
        PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_SIG_SYSTEM_ERROR | PCI_STATUS_DETECTED_PARITY);

    if (type == PCI_HEADER_TYPE_BRIDGE) {
        // PCI_PRIMARY_BUS, PCI_SECONDARY_BUS, PCI_SUBORDINATE_BUS and PCI_SEC_LETENCY_TIMER
        memset(m_writeMask + PCI_PRIMARY_BUS, 0xff, 4);

        // base and limit
        Write8(m_writeMask, PCI_IO_BASE, PCI_IO_RANGE_MASK & 0xff);
        Write8(m_writeMask, PCI_IO_LIMIT, PCI_IO_RANGE_MASK & 0xff);
        Write16(m_writeMask, PCI_MEMORY_BASE, PCI_MEMORY_RANGE_MASK & 0xffff);
        Write16(m_writeMask, PCI_MEMORY_LIMIT, PCI_MEMORY_RANGE_MASK & 0xffff);
        Write16(m_writeMask, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_MASK & 0xffff);
        Write16(m_writeMask, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_MASK & 0xffff);

        // PCI_PREF_BASE_UPPER32 and PCI_PREF_LIMIT_UPPER32
        memset(m_writeMask + PCI_PREF_BASE_UPPER32, 0xff, 8);

        // Supported memory and i/o types
        TestAndSet8(m_configSpace, PCI_IO_BASE, PCI_IO_RANGE_TYPE_16);
        TestAndSet8(m_configSpace, PCI_IO_LIMIT, PCI_IO_RANGE_TYPE_16);
        TestAndSet16(m_configSpace, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_TYPE_64);
        TestAndSet16(m_configSpace, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_TYPE_64);

        /*
         * TODO: Bridges default to 10-bit VGA decoding but we currently only
         * implement 16-bit decoding (no alias support).
         */
        Write16(m_writeMask, PCI_BRIDGE_CONTROL,
            PCI_BRIDGE_CTL_PARITY |
            PCI_BRIDGE_CTL_SERR |
            PCI_BRIDGE_CTL_ISA |
            PCI_BRIDGE_CTL_VGA |
            PCI_BRIDGE_CTL_VGA_16BIT |
            PCI_BRIDGE_CTL_MASTER_ABORT |
            PCI_BRIDGE_CTL_BUS_RESET |
            PCI_BRIDGE_CTL_FAST_BACK |
            PCI_BRIDGE_CTL_DISCARD |
            PCI_BRIDGE_CTL_SEC_DISCARD |
            PCI_BRIDGE_CTL_DISCARD_SERR);

        // Below does not do anything as we never set this bit, put here for completeness
        Write16(m_write1ToClearMask, PCI_BRIDGE_CONTROL, PCI_BRIDGE_CTL_DISCARD_STATUS);
        TestAndSet8(m_checkMask, PCI_IO_BASE, PCI_IO_RANGE_TYPE_MASK);
        TestAndSet8(m_checkMask, PCI_IO_LIMIT, PCI_IO_RANGE_TYPE_MASK);
        TestAndSet16(m_checkMask, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_TYPE_MASK);
        TestAndSet16(m_checkMask, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_TYPE_MASK);
    }
}

PCIDevice::~PCIDevice() {
}

bool PCIDevice::GetIOBar(uint32_t port, uint8_t* barIndex, uint32_t *baseAddress) {
    uint8_t headerType = m_configSpace[PCI_HEADER_TYPE];
    uint8_t numBARs;

    switch (headerType) {
    case PCI_HEADER_TYPE_NORMAL:
    case PCI_HEADER_TYPE_MULTIFUNCTION: {
        numBARs = PCI_NUM_BARS_DEVICE;
        break;
    }
    case PCI_HEADER_TYPE_BRIDGE: {
        numBARs = PCI_NUM_BARS_PCI_BRIDGE;
        break;
    }
    default: {
        log_warning("PCIDevice::GetIOBar: Invalid device type 0x%x\n", headerType);
        return false;
    }
    }

    for (uint8_t i = 0; i < numBARs; i++) {
        if (m_BARSizes[i] == 0) {
            continue;
        }
        
        uint32_t barValue = Read32(m_configSpace, PCI_BASE_ADDRESS_0 + i * sizeof(PCIBarRegister));
        PCIBarRegister *bar = reinterpret_cast<PCIBarRegister *>(&barValue);
        if ((bar->Raw.type & PCI_BAR_TYPE_MASK) != PCI_BAR_TYPE_IO) {
            // BAR is not I/O
            continue;
        }
        
        uint32_t barAddr = bar->IO.address << 2;
        if (port >= barAddr && port < barAddr + m_BARSizes[i]) {
            *barIndex = i;
            *baseAddress = barAddr;
            return true;
        }
    }

    return false;
}

bool PCIDevice::GetMMIOBar(uint32_t addr, uint8_t* barIndex, uint32_t *baseAddress) {
    uint8_t headerType = m_configSpace[PCI_HEADER_TYPE];
    uint8_t numBARs;

    switch (headerType) {
    case PCI_HEADER_TYPE_NORMAL:
    case PCI_HEADER_TYPE_MULTIFUNCTION: {
        numBARs = PCI_NUM_BARS_DEVICE;
        break;
    }
    case PCI_HEADER_TYPE_BRIDGE: {
        numBARs = PCI_NUM_BARS_PCI_BRIDGE;
        break;
    }
    default: {
        log_warning("PCIDevice::GetMMIOBar: Invalid device type 0x%x\n", headerType);
        return false;
    }
    }

    for (uint8_t i = 0; i < numBARs; i++) {
        if (m_BARSizes[i] == 0) {
            continue;
        }
        
        uint32_t barValue = Read32(m_configSpace, PCI_BASE_ADDRESS_0 + i * sizeof(PCIBarRegister));
        PCIBarRegister *bar = reinterpret_cast<PCIBarRegister *>(&barValue);
        if ((bar->Raw.type & PCI_BAR_TYPE_MASK) != PCI_BAR_TYPE_MEMORY) {
            // BAR is not memory
            continue;
        }

        uint32_t barAddr = bar->Memory.address << 4;
        if (addr >= barAddr && addr < barAddr + m_BARSizes[i]) {
            *barIndex = i;
            *baseAddress = barAddr;
            return true;
        }
    }

    return false;
}

bool PCIDevice::RegisterBAR(int index, uint32_t size, uint32_t type) {
    uint8_t headerType = Read8(m_configSpace, PCI_HEADER_TYPE);
    uint8_t numBARs;

    switch (headerType) {
    case PCI_HEADER_TYPE_NORMAL:
    case PCI_HEADER_TYPE_MULTIFUNCTION: {
        numBARs = PCI_NUM_BARS_DEVICE;
        break;
    }
    case PCI_HEADER_TYPE_BRIDGE: {
        numBARs = PCI_NUM_BARS_PCI_BRIDGE;
        break;
    }
    default: {
        log_warning("PCIDevice::RegisterBAR: invalid device type 0x%x\n", headerType);
        return false;
    }
    }

    if (index >= numBARs) {
        log_warning("PCIDevice::RegisterBAR: BAR index out of bounds (%d >= %d)\n", index, numBARs);
        return false;
    }

    if (m_BARSizes[index] != 0) {
        log_warning("PCIDevice::RegisterBAR: Trying to register a BAR that is already allocated (index: %d)\n", index);
        return false;
    }

    if (size & (size - 1)) {
        log_warning("PCIDevice::RegisterBAR: Size is not a power of 2 (0x%x)\n", size);
    }

    m_BARSizes[index] = size;
    uint8_t addr = index * sizeof(PCIBarRegister) + PCI_BASE_ADDRESS_0;
    Write32(m_configSpace, addr, type);
    Write32(m_writeMask, addr, ~(size - 1));
    Write32(m_checkMask, addr, 0xFFFFFFFF);

    return true;
}

void PCIDevice::ReadConfig(uint32_t reg, void *value, uint8_t size) {
    memcpy(value, &m_configSpace[reg], size);
}

void PCIDevice::WriteConfig(uint32_t reg, uint32_t value, uint8_t size) {
    for (uint8_t i = 0; i < size; value >>= 8, ++i) {
        uint8_t wmask = m_writeMask[reg + i];
        uint8_t w1cmask = m_write1ToClearMask[reg + i];
        assert(!(wmask & w1cmask));
        m_configSpace[reg + i] = (m_configSpace[reg + i] & ~wmask) | (value & wmask);
        m_configSpace[reg + i] &= ~(value & w1cmask); // W1C: Write 1 to Clear
    }

    // TODO: handle memory mapping changes
    // TODO: handle Message Signalled Interrupts
}

void PCIDevice::ChangeIRQLevel(uint8_t irqNum, int change) {
    PCIDevice *dev = this;

    PCIBus *bus;
    for (;;) {
        bus = dev->m_bus;
        irqNum = bus->MapIRQ(dev, irqNum);
        if (bus->CanSetIRQ()) {
            break;
        }
        dev = bus->m_owner;
    }
    bus->m_irqCount[irqNum] += change;
    bus->SetIRQ(irqNum, bus->m_irqCount[irqNum] != 0);
}

void PCIDevice::UpdateIRQStatus() {
    if (m_irqState != 0) {
        m_configSpace[PCI_STATUS] |= PCI_STATUS_INTERRUPT;
    }
    else {
        m_configSpace[PCI_STATUS] &= ~PCI_STATUS_INTERRUPT;
    }
}

void PCIDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("PCIDevice::PCIIORead:  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
    *value = 0;
}

void PCIDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_spew("PCIDevice::PCIIOWrite: bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

void PCIDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    log_spew("PCIDevice::PCIMMIORead:  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
    *value = 0;
}

void PCIDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    log_spew("PCIDevice::PCIMMIOWrite: bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

}
