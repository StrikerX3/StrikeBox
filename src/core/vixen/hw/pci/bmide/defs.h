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

enum CommandRegisterBits {
    CmdReadWriteControl = (1 << 3),     // Read/Write Control: 0 = read, 1 = write
    CmdStartStopBusMaster = (1 << 0),   // Start/Stop Bus Master: 1 = start, 0 = stop (edge detected)
};

const uint8_t kCommandRegMask = CmdReadWriteControl | CmdStartStopBusMaster;

enum StatusRegisterBits {
    StSimplexOnly = (1 << 7),          // 0 = duplex (primary and secondary channels are independent), 1 = simplex
    StDrive1DMACapable = (1 << 6),     // Drive 1 is capable of DMA transfers
    StDrive0DMACapable = (1 << 5),     // Drive 0 is capable of DMA transfers
    StInterrupt = (1 << 2),            // Interrupt level (set to 1 when IDE raises the interrupt line, write 1 to clear)
    StError = (1 << 1),                // Error (set when a command fails, write 1 to clear)
    StBusMasterIDEActive = (1 << 0),   // Indicates ongoing transfer
};

const uint8_t kStatusRegMask = StSimplexOnly | StDrive1DMACapable | StDrive0DMACapable | StInterrupt | StError | StBusMasterIDEActive;
const uint8_t kStatusRegWriteClearMask = StInterrupt | StError;
const uint8_t kStatusRegWriteMask = StDrive1DMACapable | StDrive0DMACapable;

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
