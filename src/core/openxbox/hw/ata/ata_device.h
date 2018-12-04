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

#include "openxbox/cpu.h"
#include "../ata/defs.h"
#include "ata_common.h"
#include "drvs/ata_device_driver.h"
#include "drvs/ata_device_driver_null.h"

namespace openxbox {
namespace hw {
namespace ata {

/*!
 * Represents one ATA device. The concrete implementation for the device is
 * provided by the IATADeviceDriver.
 */
class ATADevice {
public:
    ATADevice(Channel channel, uint8_t devIndex, ATARegisters& regs);
    ~ATADevice();

    // ----- Device driver management -----------------------------------------

    bool IsAttached() const { return m_driver->IsAttached(); }
    void SetDeviceDriver(IATADeviceDriver *driver) { m_driver = driver; }

    // ----- PIO data buffer --------------------------------------------------

    uint32_t ReadBuffer(uint8_t *dest, uint32_t length);

    // ----- Command handlers -------------------------------------------------
    // These functions must return false on error

    bool IdentifyDevice();     // [8.12] 0xEC   Identify Device
    bool SetFeatures();        // [8.37] 0xEF   Set Features

    // ----- Set Features subcommand handlers ---------------------------------

    bool SetTransferMode();
    bool SetPIOTransferMode(PIOTransferType type, uint8_t mode);
    bool SetDMATransferMode(DMATransferType type, uint8_t mode);

private:
    friend class ATAChannel;

    Channel m_channel;
    uint8_t m_devIndex;

    // The device driver that responds to commands
    IATADeviceDriver *m_driver;

    // ----- Command handler implementations ----------------------------------
    
    bool __doIdentifyDevice();
    bool __doSetFeatures();

    // ----- Registers --------------------------------------------------------

    // A reference to the registers of the ATA channel that owns this device
    ATARegisters& m_regs;

    // [8.37.10] PIO and DMA modes are separate
    PIOTransferType m_pioTransferType = XferTypePIODefault;
    uint8_t m_pioTransferMode = 0;

    DMATransferType m_dmaTransferType = XferTypeMultiWordDMA;
    uint8_t m_dmaTransferMode = 0;

    // ----- Data buffer ------------------------------------------------------

    uint8_t *m_dataBuffer = nullptr;
    uint32_t m_dataBufferSize = 0;
    uint32_t m_dataBufferPos = 0;

    /*!
     * Initializes a data buffer of the specified size. If the current buffer
     * is not large enough to fit the requested number of bytes, a new buffer
     * is allocated in memory, replacing the existing buffer.
     */
    void InitDataBuffer(uint32_t dataBufferSize);
};

}
}
}
