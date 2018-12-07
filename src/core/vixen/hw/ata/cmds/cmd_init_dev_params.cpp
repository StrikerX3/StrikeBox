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
#include "cmd_init_dev_params.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

InitializeDeviceParameters::InitializeDeviceParameters(ATADevice& device)
    : NonDataProtocolCommand(device) {
}

InitializeDeviceParameters::~InitializeDeviceParameters() {
}

bool InitializeDeviceParameters::ExecuteImpl() {
    if (m_driver->SetDeviceParameters(m_regs.deviceHead & 0b1111, m_regs.sectorCount)) {
        // Handle normal output as specified in [8.16.5]

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //  "DF (Device Fault) shall be cleared to zero."
        //  "DRQ shall be cleared to zero."
        //  "ERR shall be cleared to zero."
        m_regs.status &= ~(StBusy | StDeviceFault | StDataRequest | StError);
    }
    else {
        // Handle normal output as specified in [8.16.6]

        // Error register:
        //  "ABRT shall be set to one if the device does not support the requested CHS translation."
        //  "ABRT may be set to one if the device is not able to complete the action requested by the command."
        m_regs.error |= ErrAbort;

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "DF (Device Fault) shall be set to one if a device fault has occurred."
        //    TODO: what constitutes a device fault in this case?

        //  "BSY shall be cleared to zero indicating command completion."
        //  "DRQ shall be cleared to zero."
        m_regs.status &= ~(StBusy | StDataRequest);

        //  "ERR shall be set to one if an Error register bit is set to one."
        if (m_regs.error) {
            m_regs.status |= StError;
        }
    }

    return true;
}

}
}
}
}
