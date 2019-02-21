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
#include "vixen/hw/ata/cmds/cmd_identify_device.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

IdentifyDevice::IdentifyDevice(ATADevice& device)
    : PIODataInProtocolCommand(device) {
}

IdentifyDevice::~IdentifyDevice() {
}

bool IdentifyDevice::HasMoreData() {
    return false;
}

BlockReadResult IdentifyDevice::ReadBlock(uint8_t data[kSectorSize]) {
    // [8.12.7] As a prerequisite, DRDY must be set equal to one
    if ((m_regs.status & StReady) == 0) {
        return BlockReadError;
    }

    // Check if the device supports the PACKET Command feature set
    if (m_driver->SupportsPacketCommands()) {
        // Respond as specified in [8.12.5.2], which follows the
        // Signature and Persistence protocol [9.1]
        m_regs.WriteSignature(true);
        m_regs.error |= ErrAbort;
        return BlockReadError;
    }

    // Ask the device driver to identify itself
    m_driver->IdentifyDevice(reinterpret_cast<IdentifyDeviceData *>(data));

    // The documentation does not specify error output for this command when
    // the device does not support the PACKET Command feature set.
    // Assume this command never fails.

    // Handle normal output as specified in [8.12.5.1]

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
