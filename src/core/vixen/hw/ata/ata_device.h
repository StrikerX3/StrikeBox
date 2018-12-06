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

#include "vixen/cpu.h"
#include "../ata/ata_defs.h"
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
    ATADevice(Channel channel, uint8_t devIndex, ATARegisters& regs);
    ~ATADevice();

    // ----- Device driver management -----------------------------------------

    void SetDeviceDriver(IATADeviceDriver *driver) { m_driver = driver; }
    bool IsAttached() const { return m_driver->IsAttached(); }

    // ----- PIO data buffer --------------------------------------------------

    uint32_t ReadBuffer(uint8_t *dst, uint32_t length);
    uint32_t WriteBuffer(uint8_t *src, uint32_t length);
    bool IsBlockTransferComplete();
    bool RequestNextBlock();

    // ----- DMA transfer -----------------------------------------------------

    bool ReadDMA(uint8_t dstBuffer[kSectorSize]);
    bool WriteDMA(uint8_t srcBuffer[kSectorSize]);
    bool IsDMAFinished() { return m_dma_currentLBA >= m_dma_endingLBA; }
    void EndDMA();
    
    // ----- Transfer control -------------------------------------------------
    
    bool HasTransferError();
    void FinishCommand();

    // ----- Command handlers -------------------------------------------------
    // These functions must return false on error

    bool IdentifyDevice();               // [8.12] 0xEC   Identify Device
    bool IdentifyPACKETDevice();         // [8.13] 0xA1   Identify PACKET Device
    bool InitializeDeviceParameters();   // [8.16] 0x91   Initialize Device Parameters
    bool BeginReadDMA();                 // [8.23] 0xC8   Read DMA
    bool BeginSecurityUnlock();          // [8.34] 0xF2   Security Unlock
    bool SetFeatures();                  // [8.37] 0xEF   Set Features
    bool BeginWriteDMA();                // [8.45] 0xCA   Write DMA

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

    // ----- Registers --------------------------------------------------------

    // A reference to the registers of the ATA channel that owns this device
    ATARegisters& m_regs;

    // [8.37.10] PIO and DMA modes are separate
    PIOTransferType m_pioTransferType = XferTypePIODefault;
    uint8_t m_pioTransferMode = 0;

    DMATransferType m_dmaTransferType = XferTypeMultiWordDMA;
    uint8_t m_dmaTransferMode = 0;

    // ----- Signature and Persistence [9.1] ----------------------------------

    void WriteSignature(bool packetFeatureSet);

    // ----- Data buffer (for PIO transfers) ----------------------------------

    uint8_t m_dataBuffer[kSectorSize];
    uint32_t m_dataBufferPos;

    uint32_t GetRemainingBufferLength();

    // ----- Data transfer ----------------------------------------------------

    // true if any transfer is in progress (PIO or DMA)
    bool m_transferActive;
    
    // How many sectors to read/write
    uint32_t m_sectorsRemaining;

    // Whether the command reported an error
    bool m_transferError;
    
    // The command currently executing
    bool m_transferHasCommand;
    Command m_command;

    // Execute the command after receiving a full sector of data
    void ExecuteCommand();

    // ----- DMA transfer -----------------------------------------------------
    
    // Parameters
    uint32_t m_dma_startingLBA;
    uint32_t m_dma_endingLBA;    // Exclusive
    bool m_dma_isWrite;   // Current DMA operation type (true = write, false = read), used for sanity check
  
    // State
    uint8_t m_dma_currentLBA;

    bool HandleReadDMA(uint8_t dstBuffer[kSectorSize]);
    bool HandleWriteDMA(uint8_t srcBuffer[kSectorSize]);
};

}
}
}
