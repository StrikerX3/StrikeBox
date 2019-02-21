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
#pragma once

#include <cstdint>

#include "../ata/ata_defs.h"
#include "../basic/interrupt.h"
#include "ata_common.h"
#include "drvs/ata_device_driver.h"
#include "drvs/drv_null.h"

namespace vixen {
namespace hw {
namespace ata {

/*!
 * Represents one ATA device. The concrete implementation for the device is
 * provided by the IATADeviceDriver.
 */
class ATADevice {
public:
    ATADevice(Channel channel, uint8_t devIndex, ATARegisters& regs, InterruptTrigger& interrupt);
    ~ATADevice();

    ATARegisters& GetRegisters() { return m_regs; }
    IATADeviceDriver* GetDriver() { return m_driver; }
    const Channel GetChannel() const { return m_channel; }
    const uint8_t GetIndex() const { return m_devIndex; }
    InterruptTrigger& GetInterrupt() const { return m_interrupt; }

    // ----- Device driver management -----------------------------------------

    void SetDeviceDriver(IATADeviceDriver *driver) { m_driver = driver; }
    bool IsAttached() const { return m_driver->IsAttached(); }

private:
    friend class ATAChannel;

    const Channel m_channel;
    const uint8_t m_devIndex;

    InterruptTrigger& m_interrupt;

    // The device driver that responds to commands
    IATADeviceDriver *m_driver;

    // ----- Registers --------------------------------------------------------

    // A reference to the registers of the ATA channel that owns this device
    ATARegisters& m_regs;
};

}
}
}
