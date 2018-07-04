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
// *   Cxbx->devices->USBController->USBDevice.h
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

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "ohci_common.h"
#include "openxbox/cpu.h"

namespace openxbox {

typedef enum USBDeviceFlags {
    USB_DEV_FLAG_FULL_PATH,
    USB_DEV_FLAG_IS_HOST,
    USB_DEV_FLAG_MSOS_DESC_ENABLE,
    USB_DEV_FLAG_MSOS_DESC_IN_USE,
}
USBDeviceFlags;

// Forward declare OHCI class for m_HostController pointer
class OHCI;

/* Helper class which provides various functionality to both OHCI and usb device classes */
class USBPCIDevice : public PCIDevice {
public:
    // constructor
    USBPCIDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID, uint8_t m_irqn, Cpu *cpu);
    virtual ~USBPCIDevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;



    // USBDevice-specific functions/variables
    // pointer to the host controller this device refers to
    OHCI* m_HostController;
    // PCI path of this usb device
    const char* m_PciPath;

    // register a port with the HC
    void USB_RegisterPort(USBPort* Port, int Index, int SpeedMask, USBPortOps* Ops);
    // reset a usb port
    void USB_PortReset(USBPort* Port);
    // update device status during an attach
    void USB_DeviceAttach(XboxDeviceState* dev);
    // update port status when a device is attached
    void USB_Attach(USBPort* Port);
    // update port status when a device is detached
    void USB_Detach(USBPort* Port);
    // a device downstream from the device attached to the port (attached through a hub) is detached
    void ChildDetach(USBPort* Port, XboxDeviceState* Child);
    // TODO
    void Wakeup(USBPort* Port);
    // TODO
    void Complete(USBPort* Port, USBPacket *P);
    // reset a device
    void USB_DeviceReset(XboxDeviceState* Dev);
    //
    XboxDeviceState* USB_DeviceFindDevice(XboxDeviceState* Dev, uint8_t Addr);
    // find the requested endpoint in the supplied device
    USBEndpoint* USB_GetEP(XboxDeviceState* Dev, int Pid, int Ep);
    // setup a packet for transfer
    void USB_PacketSetup(USBPacket* p, int Pid, USBEndpoint* Ep, unsigned int Stream,
        uint64_t Id, bool ShortNotOK, bool IntReq);
    // check if the state of the packet is queued or async
    bool USB_IsPacketInflight(USBPacket* p);
    // append the user buffer to the packet
    void USB_PacketAddBuffer(USBPacket* p, void* ptr, size_t len);
    // transfer and process the packet
    void USB_HandlePacket(XboxDeviceState* dev, USBPacket* p);
    // check if the packet has the expected state and assert if not
    void USB_PacketCheckState(USBPacket* p, USBPacketState expected);
    // process the packet
    void USB_ProcessOne(USBPacket* p);
    //
    void USB_DoParameter(XboxDeviceState* s, USBPacket* p);
    // process a setup token
    void USB_DoTokenSetup(XboxDeviceState* s, USBPacket* p);
    // process an input token
    void DoTokenIn(XboxDeviceState* s, USBPacket* p);
    // process an output token
    void DoTokenOut(XboxDeviceState* s, USBPacket* p);
    // copy the packet data to the buffer pointed to by ptr
    void USB_PacketCopy(USBPacket* p, void* ptr, size_t bytes);
    // Cancel an active packet.  The packed must have been deferred by
    // returning USB_RET_ASYNC from handle_packet, and not yet completed
    void USB_CancelPacket(USBPacket* p);
    // queue a packet to an endpoint
    void USB_QueueOne(USBPacket* p);
    // call usb class init function
    int USB_DeviceInit(XboxDeviceState* dev);
    // call usb class find_device function
    XboxDeviceState * USB_FindDevice(USBPort* Port, uint8_t Addr);
    // call usb class cancel_packet function
    void USB_DeviceCancelPacket(XboxDeviceState* dev, USBPacket* p);
    // call usb class handle_destroy function
    void USB_DeviceHandleDestroy(XboxDeviceState* dev);
    // call usb class handle_attach function
    void USB_DeviceHandleAttach(XboxDeviceState* dev);
    // call usb class handle_reset function
    void USB_DeviceHandleReset(XboxDeviceState* dev);
    // call usb class handle_control function
    void USB_DeviceHandleControl(XboxDeviceState* dev, USBPacket* p, int request, int value, int index, int length, uint8_t* data);
    // call usb class handle_data function
    void USB_DeviceHandleData(XboxDeviceState* dev, USBPacket* p);
    // call usb class set_interface function
    void USB_DeviceSetInterface(XboxDeviceState* dev, int iface, int alt_old, int alt_new);
    // call usb class flush_ep_queue function
    void USB_DeviceFlushEPqueue(XboxDeviceState* dev, USBEndpoint* ep);
    // call usb class ep_stopped function
    void USB_DeviceEPstopped(XboxDeviceState* Dev, USBEndpoint* Ep);
    // set the type of the endpoint
    void USB_EPsetType(XboxDeviceState* dev, int pid, int ep, uint8_t type);
    // set the interface number of the endpoint
    uint8_t USB_EPsetIfnum(XboxDeviceState* dev, int pid, int ep, uint8_t ifnum);
    // set the maximum packet size parameter of the endpoint
    void USB_EPsetMaxPacketSize(XboxDeviceState* dev, int pid, int ep, uint16_t raw);
    // assign port numbers (also for hubs)
    void USB_PortLocation(USBPort* downstream, USBPort* upstream, int portnr);
private:
    uint8_t m_irqn;
    Cpu* m_cpu;
};

}
