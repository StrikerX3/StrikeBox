// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#include "bmide.h"
#include "vixen/log.h"

namespace vixen {

using namespace hw::bmide;

BMIDEDevice::BMIDEDevice(uint8_t *ram, uint32_t ramSize)
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01BC, 0xD2,
        0x01, 0x01, 0x8A) // IDE controller
    , m_ram(ram)
    , m_ramSize(ramSize)
{
}

BMIDEDevice::~BMIDEDevice() {
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
    case RegPrimaryCommand: ReadCommand(ChanPrimary, value, size); break;
    case RegPrimaryStatus: ReadStatus(ChanPrimary, value, size); break;
    case RegPrimaryPRDTableAddress: ReadPRDTableAddress(ChanPrimary, value, size); break;  // TODO: handle unaligned accesses
    case RegSecondaryCommand: ReadCommand(ChanSecondary, value, size); break;
    case RegSecondaryStatus: ReadStatus(ChanSecondary, value, size); break;
    case RegSecondaryPRDTableAddress: ReadPRDTableAddress(ChanSecondary, value, size); break;  // TODO: handle unaligned accesses
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
    case RegPrimaryCommand: WriteCommand(ChanPrimary, value, size); break;
    case RegPrimaryStatus: WriteStatus(ChanPrimary, value, size); break;
    case RegPrimaryPRDTableAddress: WritePRDTableAddress(ChanPrimary, value, size); break;
    case RegSecondaryCommand: WriteCommand(ChanSecondary, value, size); break;
    case RegSecondaryStatus: WriteStatus(ChanSecondary, value, size); break;
    case RegSecondaryPRDTableAddress: WritePRDTableAddress(ChanSecondary, value, size); break;
    default:
        log_spew("BMIDEDevice::PCIIOWrite:  Unimplemented!  bar = %d,  port = 0x%x,  value = 0x%x,  size = %u\n", barIndex, port, value, size);
        break;
    }
}

void BMIDEDevice::ReadCommand(Channel channel, uint32_t *value, uint8_t size) {
    *value = m_command[channel] & kCommandRegMask;
}

void BMIDEDevice::ReadStatus(Channel channel, uint32_t *value, uint8_t size) {
    *value = m_status[channel] & kStatusRegMask;
}

void BMIDEDevice::ReadPRDTableAddress(Channel channel, uint32_t *value, uint8_t size) {
    if (size == 1) {
        *(uint8_t*)value = m_prdTableAddrs[channel];
    }
    else if (size == 2) {
        *(uint16_t*)value = m_prdTableAddrs[channel];
    }
    else {
        *value = m_prdTableAddrs[channel];
    }
}

void BMIDEDevice::WriteCommand(Channel channel, uint32_t value, uint8_t size) {
    log_spew("BMIDEDevice::WriteCommand:  Unimplemented!  channel = %d,  value = 0x%x,  size = %u\n", channel, value, size);
    m_command[channel] = value;
}

void BMIDEDevice::WriteStatus(Channel channel, uint32_t value, uint8_t size) {
    // Clear interrupt and error flags if requested
    m_status[channel] &= ~(value & kStatusRegWriteClearMask);

    // Update writable bits
    m_status[channel] &= ~kStatusRegWriteMask;
    m_status[channel] |= value & kStatusRegWriteMask;
}

void BMIDEDevice::WritePRDTableAddress(Channel channel, uint32_t value, uint8_t size) {
    // Clear least significant bit, which must always be zero
    value &= ~1;

    // Check for unaligned address
    if (value & (sizeof(uint32_t) - 1)) {
        log_warning("BMIDEDevice::WritePRDTableAddress:  Guest wrote unaligned PRD table address: 0x%x\n", value);
    }

    // Update register value
    if (size == 1) {
        m_prdTableAddrs[channel] = (uint8_t)value;
    }
    else if (size == 2) {
        m_prdTableAddrs[channel] = (uint16_t)value;
    }
    else {
        m_prdTableAddrs[channel] = value;
    }
}

}
