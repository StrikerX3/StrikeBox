/*
 * Portions of the code are based on Cxbx-Reloaded's APU emulator.
 * The original copyright header is included below.
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->devices->APUDevice.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2018 Luke Usher <luke.usher@outlook.coM>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "nvapu.h"
#include "vixen/log.h"

namespace vixen {

uint32_t GetAPUTime() {
    // This timer counts at 48000 Hz
    auto t = std::chrono::high_resolution_clock::now();
    return static_cast<uint32_t>(t.time_since_epoch().count() * 0.000048000);
}

// TODO: Everything :P
// TODO: Audio Processing/Thread

NVAPUDevice::NVAPUDevice()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B0, 0xB1,
        0x04, 0x01, 0x00) // Multimedia Audio Controller
{
}

NVAPUDevice::~NVAPUDevice() {
}

// PCI Device functions

void NVAPUDevice::Init() {
    RegisterBAR(0, 0x80000, PCI_BAR_TYPE_MEMORY); // 0xFE800000 - 0xFE87FFFF

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x01);
    Write8(m_configSpace, PCI_MAX_LAT, 0x0c);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x2);
    Write32(m_configSpace, 0x4c, 0x50a);
    for (uint8_t i = 0; i < 0x100 - 0x50; i += 4) {
        Write32(m_configSpace, 0x50 + i, 0x20001);
    }
}

void NVAPUDevice::Reset() {
}

void NVAPUDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("NVAPUDevice::PCIIORead:   Unimplemented!  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
}

void NVAPUDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::PCIIOWrite:  Unimplemented!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
}

void NVAPUDevice::PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) {
    if (addr >= APU_VP_BASE && addr < APU_VP_BASE + APU_VP_SIZE) {
        VPRead(addr - APU_VP_BASE, value, size);
        return;
    }

    if (addr >= APU_GP_BASE && addr < APU_GP_BASE + APU_GP_SIZE) {
        GPRead(addr - APU_GP_BASE, value, size);
        return;
    }

    if (addr >= APU_EP_BASE && addr < APU_EP_BASE + APU_EP_SIZE) {
        EPRead(addr - APU_EP_BASE, value, size);
        return;
    }

    // FIXME: this satisfies the most basic needs of the guest software
    if (addr == 0x200C) {
        *value = GetAPUTime();
    }

    log_spew("NVAPUDevice::PCIMMIORead:   Unhandled read!  bar = %d,  address = 0x%x,  size = %u\n", barIndex, addr, size);
}

void NVAPUDevice::PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) {
    if (addr >= APU_VP_BASE && addr < APU_VP_BASE + APU_VP_SIZE) {
        VPWrite(addr - APU_VP_BASE, value, size);
        return;
    }

    if (addr >= APU_GP_BASE && addr < APU_GP_BASE + APU_GP_SIZE) {
        VPWrite(addr - APU_GP_BASE, value, size);
        return;
    }

    if (addr >= APU_EP_BASE && addr < APU_EP_BASE + APU_EP_SIZE) {
        VPWrite(addr - APU_EP_BASE, value, size);
        return;
    }

    log_spew("NVAPUDevice::PCIMMIOWrite:  Unhandled write!  bar = %d,  address = 0x%x,  value = 0x%x,  size = %u\n", barIndex, addr, value, size);
}

void NVAPUDevice::GPRead(uint32_t address, uint32_t *value, uint8_t size) {
    log_spew("NVAPUDevice::GPRead:   Unimplemented!  address = 0x%x,  size = %u\n", address, size);
}

void NVAPUDevice::GPWrite(uint32_t address, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::GPWrite:  Unimplemented!  address = 0x%x,  value = 0x%x,  size = %u\n", address, value, size);
}

void NVAPUDevice::EPRead(uint32_t address, uint32_t *value, uint8_t size) {
    log_spew("NVAPUDevice::EPRead:   Unimplemented!  address = 0x%x,  size = %u\n", address, size);
}

void NVAPUDevice::EPWrite(uint32_t address, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::EPWrite:  Unimplemented!  address = 0x%x,  value = 0x%x,  size = %u\n", address, value, size);
}

void NVAPUDevice::VPRead(uint32_t address, uint32_t *value, uint8_t size) {
    // FIXME: HACK: Pretend the FIFO is always empty, bypasses hangs when APU isn't fully implemented
    if (address == 0x10) {
        *value = 0x80;
        return;
    }

    log_spew("NVAPUDevice::VPRead:   Unimplemented!  address = 0x%x,  size = %u\n", address, size);
}

void NVAPUDevice::VPWrite(uint32_t address, uint32_t value, uint8_t size) {
    log_spew("NVAPUDevice::VPWrite:  Unimplemented!  address = 0x%x,  value = 0x%x,  size = %u\n", address, value, size);
}

}
