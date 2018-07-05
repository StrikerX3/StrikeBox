/*
 * Portions of the code are based on Cxbx-Reloaded's OHCI LLE implementation.
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
// *   Cxbx->devices->usb->Hub.h
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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include "ohci_common.h"

namespace openxbox {

#define NUM_PORTS 8

#define PORT_STAT_CONNECTION   0x0001
#define PORT_STAT_ENABLE       0x0002
#define PORT_STAT_SUSPEND      0x0004
#define PORT_STAT_OVERCURRENT  0x0008
#define PORT_STAT_RESET        0x0010
#define PORT_STAT_POWER        0x0100
#define PORT_STAT_LOW_SPEED    0x0200

#define PORT_STAT_C_CONNECTION	0x0001
#define PORT_STAT_C_ENABLE      0x0002
#define PORT_STAT_C_SUSPEND     0x0004
#define PORT_STAT_C_OVERCURRENT	0x0008
#define PORT_STAT_C_RESET       0x0010


/* same as Linux kernel root hubs */
typedef enum {
    STR_MANUFACTURER = 1,
    STR_PRODUCT,
    STR_SERIALNUMBER,
};

struct USBHubPort {
    USBPort port;          // downstream port status
    uint16_t wPortStatus;  // Port Status Field, in accordance with the standard
    uint16_t wPortChange;  // Port Change Field, in accordance with the standard
};

struct USBHubState {
    XboxDeviceState dev;         // hub device status
    USBEndpoint* intr;           // interrupt endpoint of the hub
    USBHubPort ports[NUM_PORTS]; // downstream ports of the hub
};

/* Class which implements a usb hub */
class Hub {
public:
    // initialize this peripheral
    int Init(int pport);
    // destructor
    ~Hub();


private:
    // usb device this hub is attached to
    USBPCIDevice* m_UsbDev = nullptr;
    // hub state
    USBHubState* m_HubState = nullptr;
    // hub class functions
    USBDeviceClass* m_pPeripheralFuncStruct = nullptr;

    // initialize various member variables/functions
    XboxDeviceState* ClassInitFn();
    // see USBDeviceClass for comments about these functions
    int UsbHub_Initfn(XboxDeviceState* dev);
    XboxDeviceState* UsbHub_FindDevice(XboxDeviceState* dev, uint8_t addr);
    void UsbHub_HandleReset();
    void UsbHub_HandleControl(XboxDeviceState* dev, USBPacket* p,
        int request, int value, int index, int length, uint8_t* data);
    void UsbHub_HandleData(XboxDeviceState* dev, USBPacket* p);
    void UsbHub_HandleDestroy(XboxDeviceState* dev);
    // see USBPortOps struct for info
    void UsbHub_Attach(USBPort* port1);
    void UsbHub_Detach(USBPort* port1);
    void UsbHub_ChildDetach(XboxDeviceState* child);
    void UsbHub_Wakeup(USBPort* port1);
    void UsbHub_Complete(USBPort* port, USBPacket* packet);
    // reserve a usb port for this hub
    int UsbHubClaimPort(XboxDeviceState* dev, int port);
    // free the usb port used by this hub
    void UsbHubReleasePort(XboxDeviceState* dev);
};

extern Hub* g_HubObjArray[4];

}
