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

#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"

namespace vixen {

#define APU_VP_BASE 0x20000
#define APU_VP_SIZE 0x10000

#define APU_GP_BASE 0x30000
#define APU_GP_SIZE 0x10000

#define APU_EP_BASE 0x50000
#define APU_EP_SIZE 0x10000

class NVAPUDevice : public PCIDevice {
public:
    // constructor
    NVAPUDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
    virtual ~NVAPUDevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;

private:
    void GPRead(uint32_t address, uint32_t *value, uint8_t size);
    void GPWrite(uint32_t address, uint32_t value, uint8_t size);

    void EPRead(uint32_t address, uint32_t *value, uint8_t size);
    void EPWrite(uint32_t address, uint32_t value, uint8_t size);

    void VPRead(uint32_t address, uint32_t *value, uint8_t size);
    void VPWrite(uint32_t address, uint32_t value, uint8_t size);
};

}
