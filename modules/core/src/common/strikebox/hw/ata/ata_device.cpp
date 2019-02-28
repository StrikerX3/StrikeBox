// ATA/ATAPI-4 emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement a subset of the ATA/ATAPI-4 specification
// that satisifies the requirements of an IDE interface for the Original Xbox.
//
// Specification:
// http://www.t13.org/documents/UploadedDocuments/project/d1153r18-ATA-ATAPI-4.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification.
#include "strikebox/hw/ata/ata_device.h"

#include "strikebox/log.h"
#include "strikebox/io.h"

namespace strikebox {
namespace hw {
namespace ata {

ATADevice::ATADevice(Channel channel, uint8_t devIndex, ATARegisters& regs, InterruptTrigger& interrupt)
    : m_channel(channel)
    , m_devIndex(devIndex)
    , m_driver(&g_nullATADeviceDriver)
    , m_regs(regs)
    , m_interrupt(interrupt)
{
}

ATADevice::~ATADevice() {
}

}
}
}
