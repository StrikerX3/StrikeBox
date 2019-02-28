/*
* Portions of the code are based on QEMU's AMD756 SMBus implementation.
* The original copyright header is included below.
*/
/*
 * AMD756 SMBus implementation
 *
 * Copyright (C) 2012 espes
 *
 * Based on pm_smbus.c
 * Copyright (c) 2006 Fabrice Bellard
 * Based on Linux drivers/i2c/busses/i2c-amd756.c
 * Copyright (c) 1999-2002 Merlin Hughes <merlin@merlin.org>
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
#include "strikebox/hw/bus/smbus.h"

#include "strikebox/log.h"

namespace strikebox {

SMBus::SMBus(IRQ *irq)
	: PCIDevice(PCI_HEADER_TYPE_MULTIFUNCTION, PCI_VENDOR_ID_NVIDIA, 0x01B4, 0xB1,
		0x0c, 0x05, 0x00) // SMBus
    , m_irq(irq)
{
    m_Status = 0;
}

SMBus::~SMBus() {
}

void SMBus::Init() {
    RegisterBAR(0, 0x10, PCI_BAR_TYPE_IO); // 0x0 - 0xF  (never used)
    RegisterBAR(1, 0x10, PCI_BAR_TYPE_IO); // 0xC000 - 0xC00F
    RegisterBAR(2, 0x20, PCI_BAR_TYPE_IO); // 0xC200 - 0xC21F

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x03);
    Write8(m_configSpace, PCI_MAX_LAT, 0x01);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x2);
}

void SMBus::Reset() {
    m_Status = 0;

    for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
        it->second->Reset();
    }
}

void SMBus::ConnectDevice(uint8_t addr, SMDevice *pDevice) {
    uint8_t dev_addr = (addr >> 1) & 0x7f;

    if (m_Devices.find(dev_addr) != m_Devices.end()) {
        log_warning("SMBus: Attempting to connect two devices to the same device address\n");
        return;
    }

    m_Devices[dev_addr] = pDevice;
    pDevice->Init();
}

void SMBus::ExecuteTransaction() {
    uint8_t prot = m_Control & GE_CYC_TYPE_MASK;
    bool read = m_Address & 0x01;
    uint8_t addr = (m_Address >> 1) & 0x7f;
    auto it = m_Devices.find(addr);
    if (it == m_Devices.end()) {
        m_Status |= GS_PRERR_STS;
        log_warning("SMBus::ExecuteTransaction: Invalid Device (Addr: 0x%x   Read: %x   Prot: 0x%x)\n", addr, read, prot);
        return;
    }

    SMDevice *pDevice = it->second;
    switch (prot) {
    case AMD756_QUICK:
        pDevice->QuickCommand(read);
        break;
    case AMD756_BYTE:
        if (read) {
            m_Data0 = pDevice->ReceiveByte();
        }
        else {
            pDevice->SendByte(m_Command);
        }
        break;
    case AMD756_BYTE_DATA:
        if (read) {
            m_Data0 = pDevice->ReadByte(m_Command);
        }
        else {
            pDevice->WriteByte(m_Command, m_Data0);
        }
        break;
    case AMD756_WORD_DATA:
        if (read) {
            uint16_t val;
            val = pDevice->ReadWord(m_Command);
            m_Data0 = val & 0xFF;
            m_Data1 = val >> 8;
        }
        else {
            pDevice->WriteWord(m_Command, (m_Data1 << 8) | m_Data0);
        }
        break;
    case AMD756_BLOCK_DATA:
        if (read) {
            m_Data0 = pDevice->ReadBlock(m_Command, m_Data);
        }
        else {
            pDevice->WriteBlock(m_Command, m_Data, m_Data0);
        }
        break;
    default:
        m_Status |= GS_PRERR_STS;
        return;
    }

    m_Status |= GS_HCYC_STS;
}

void SMBus::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    //log_spew("SMBus::PCIIORead:   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);

    if (barIndex != 1) {
        log_debug("SMBus::PCIIORead:  unimplemented access to bar %d:  port = 0x%x,  size = %u\n", barIndex, port, size);
        *value = 0;
        return;
    }

    port &= 0x3f;

    switch (port) {
    case SMB_GLOBAL_STATUS:
        *value = m_Status;
        break;
    case SMB_GLOBAL_ENABLE:
        *value = m_Control & 0x1f;
        break;
    case SMB_HOST_COMMAND:
        *value = m_Command;
        break;
    case SMB_HOST_ADDRESS:
        *value = m_Address;
        break;
    case SMB_HOST_DATA:
        if (size == 2) {
            *value = (m_Data1 << 8) | m_Data0;
        }
        else {
            *value = m_Data0;
        }
        break;
    case SMB_HOST_DATA + 1:
        *value = m_Data1;
        break;
    case SMB_HOST_BLOCK_DATA:
        *value = m_Data[m_Index++];
        if (m_Index > 31) {
            m_Index = 0;
        }
        break;
    default:
        log_debug("SMBus::PCIIOWrite: Unhandled read!   bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
        *value = 0;
        break;
    }
}

void SMBus::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    //log_spew("SMBus::PCIIOWrite:  bar = %d,  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);
    
    if (barIndex != 1) {
        log_debug("SMBus::PCIIOWrite: unimplemented access to bar %d:  port = 0x%x,  size = %u,  value = 0x%x\n", barIndex, port, size, value);
        return;
    }

    port &= 0x3f;
    switch (port) {
    case SMB_GLOBAL_STATUS:
        // If a new status is being set and interrupts are enabled, trigger an interrupt
        if ((m_Control & GE_HCYC_EN) && ((value & GS_CLEAR_STS) & (~(m_Status & GS_CLEAR_STS)))) {
            m_irq->Handle(1);
        }
        else {
            m_irq->Handle(0);
        }

        if (value & GS_CLEAR_STS) {
            m_Status = 0;
            m_Index = 0;
        }
        else if (value & GS_HCYC_STS) {
            m_Status = GS_HCYC_STS;
            m_Index = 0;
        }
        else {
            m_Status = GS_HCYC_STS;
            m_Index = 0;
        }

        break;
    case SMB_GLOBAL_ENABLE:
        m_Control = value;
        if (value & GE_ABORT) {
            m_Status |= GS_ABRT_STS;
        }

        if (value & GE_HOST_STC) {
            ExecuteTransaction();

            if ((value & GE_HCYC_EN) && (m_Status & GS_CLEAR_STS)) {
                m_irq->Handle(1);
            }
        }

        break;
    case SMB_HOST_COMMAND:
        m_Command = value;
        break;
    case SMB_HOST_ADDRESS:
        m_Address = value;
        break;
    case SMB_HOST_DATA:
        if (size == 2) {
            m_Data0 = value & 0xFF;
            m_Data1 = (value >> 8) & 0xFF;
        }
        else {
            m_Data0 = value;
        }
        break;
    case SMB_HOST_DATA + 1:
        m_Data1 = value;
        break;
    case SMB_HOST_BLOCK_DATA:
        m_Data[m_Index++] = value;
        if (m_Index > 31) {
            m_Index = 0;
        }
        break;
    default:
        log_debug("SMBus::PCIIOWrite: Unhandled write!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
        break;
    }
}

}
