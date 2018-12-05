// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#pragma once

#include <cstdint>

namespace vixen {
namespace hw {
namespace bmide {

// --- Registers --------------------------------------------------------------

enum Register {
    RegPrimaryCommand = 0x00,            // Primary channel Command register
    RegPrimaryStatus = 0x02,             // Primary channel Status register
    RegPrimaryPRDTableAddress = 0x04,    // Primary channel PRD Table address (ports 04h to 07h)

    RegSecondaryCommand = 0x08,          // Secondary channel Command register
    RegSecondaryStatus = 0x0A,           // Secondary channel Status register
    RegSecondaryPRDTableAddress = 0x0C,  // Secondary channel PRD Table address (ports 04h to 07h)
};

// --- Channels ---------------------------------------------------------------

const uint8_t kNumChannels = 2;

enum Channel {
    ChanPrimary = 0,
    ChanSecondary = 1,
};

// --- Physical Region Descriptor ---------------------------------------------

// A Physical Region Descriptor (PRD) describes a range of physical memory to
// be used with data transfers. Multiple PRDs may be specified in a table; the
// end of the table is indicated by the highest bit of the last dword.
struct PhysicalRegionDescriptor {
    uint32_t basePhysicalAddress;   // Bit 0 must always be zero
    uint16_t byteCount;             // Bit 0 must always be zero
    uint16_t _reserved : 15;
    uint16_t endOfTable : 1;
};

}
}
}
