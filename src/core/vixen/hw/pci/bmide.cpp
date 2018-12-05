// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#include "bmide.h"
#include "vixen/log.h"
#include "vixen/thread.h"

namespace vixen {

using namespace hw::bmide;

BMIDEDevice::BMIDEDevice(uint8_t *ram, uint32_t ramSize)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01BC, 0xD2,
        0x01, 0x01, 0x8A) // IDE controller
    , m_ram(ram)
    , m_ramSize(ramSize)
{
    m_channels[ChanPrimary] = new BMIDEChannel(ChanPrimary);
    m_channels[ChanSecondary] = new BMIDEChannel(ChanSecondary);
}

BMIDEDevice::~BMIDEDevice() {
    for (int i = 0; i < 2; i++) {
        delete m_channels[i];
    }
}

// PCI Device functions

void BMIDEDevice::Init() {
    RegisterBAR(4, 16, PCI_BAR_TYPE_IO); // 0xFF60 - 0xFF6F
}

void BMIDEDevice::Reset() {
}

void BMIDEDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    if (barIndex != 4) {
        log_debug("BMIDEDevice::PCIIORead:  Unimplemented access to bar %d:  port = 0x%x,  size = %u\n", barIndex, port, size);
        return;
    }

    // I/O registers can be accessed as bytes, words or dwords; no size checking is needed here.

    switch (port) {
    case RegPrimaryCommand: m_channels[ChanPrimary]->ReadCommand(value, size); break;
    case RegPrimaryStatus: m_channels[ChanPrimary]->ReadStatus(value, size); break;
    case RegPrimaryPRDTableAddress: m_channels[ChanPrimary]->ReadPRDTableAddress(value, size); break;  // TODO: handle unaligned accesses
    case RegSecondaryCommand: m_channels[ChanSecondary]->ReadCommand(value, size); break;
    case RegSecondaryStatus: m_channels[ChanSecondary]->ReadStatus(value, size); break;
    case RegSecondaryPRDTableAddress: m_channels[ChanSecondary]->ReadPRDTableAddress(value, size); break;  // TODO: handle unaligned accesses
    default:
        *value = 0;
        log_spew("BMIDEDevice::PCIIORead:   Unimplemented!  bar = %d,  port = 0x%x,  size = %u\n", barIndex, port, size);
        break;
    }
}

void BMIDEDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
    if (barIndex != 4) {
        log_debug("BMIDEDevice::PCIIOWrite: Unimplemented access to bar %d:  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
        return;
    }

    // I/O registers can be accessed as bytes, words or dwords; no size checking is needed here.

    switch (port) {
    case RegPrimaryCommand: m_channels[ChanPrimary]->WriteCommand(value, size); break;
    case RegPrimaryStatus: m_channels[ChanPrimary]->WriteStatus(value, size); break;
    case RegPrimaryPRDTableAddress: m_channels[ChanPrimary]->WritePRDTableAddress(value, size); break;
    case RegSecondaryCommand: m_channels[ChanSecondary]->WriteCommand(value, size); break;
    case RegSecondaryStatus: m_channels[ChanSecondary]->WriteStatus(value, size); break;
    case RegSecondaryPRDTableAddress: m_channels[ChanSecondary]->WritePRDTableAddress(value, size); break;
    default:
        log_spew("BMIDEDevice::PCIIOWrite:  Unimplemented!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
        break;
    }
}

}
