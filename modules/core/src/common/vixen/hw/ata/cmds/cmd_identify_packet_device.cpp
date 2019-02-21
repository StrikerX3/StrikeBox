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
#include "vixen/hw/ata/cmds/cmd_identify_packet_device.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

IdentifyPacketDevice::IdentifyPacketDevice(ATADevice& device)
    : PIODataInProtocolCommand(device) {
}

IdentifyPacketDevice::~IdentifyPacketDevice() {
}

bool IdentifyPacketDevice::HasMoreData() {
    return false;
}

BlockReadResult IdentifyPacketDevice::ReadBlock(uint8_t data[kSectorSize]) {
    // Ask the device driver to identify itself
    if (!m_driver->IdentifyPacketDevice(reinterpret_cast<IdentifyPacketDeviceData *>(data))) {
        // The device does not implement the Identify PACKET Device command
        // Return command aborted as specified in the error output section [8.13.6]
        m_regs.error |= ErrAbort;
        return BlockReadError;
    }

    // Handle normal output as specified in [8.13.5]

    // Device/Head register:
    //  "DEV shall indicate the selected device."
    //     Not necessary, but the spec says so
    m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

    // Status register:
    //  "BSY shall be cleared to zero indicating command completion."
    //     Already handled by the caller

    //  "DRDY shall be set to one."
    m_regs.status |= StReady;

    //  "DF (Device Fault) shall be cleared to zero."
    //  "DRQ shall be cleared to zero."
    //  "ERR shall be cleared to zero."
    m_regs.status &= ~(StDeviceFault | StDataRequest | StError);

    return BlockReadOK;
}

}
}
}
}
