// ATAPI Command set emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement the subset of the ATAPI Command set used by the
// Original Xbox to access the DVD drive.
//
// Based on:
//   [p] SCSI Primary Commands - 3 (SPC-3) Draft
//   http://t10.org/ftp/t10/document.08/08-309r1.pdf
//
//   [m] SCSI Multimedia Commands - 3 (MMC-3) Revision 10g
//   https://www.rockbox.org/wiki/pub/Main/DataSheets/mmc2r11a.pdf
//
//   [b] SCSI Block Commands - 3 (SBC-3) Revision 25
//   http://www.13thmonkey.org/documentation/SCSI/sbc3r25.pdf
//
//   [a] SCSI Architecture Model - 3 (SAM-3) Revision 13
//   http://www.csit-sun.pub.ro/~cpop/Documentatie_SMP/Standarde_magistrale/SCSI/sam3r13.pdf
//
//   [c] ATA Packet Interface for CD-ROMs Revision 2.6 Proposed
//   http://www.bswd.com/sff8020i.pdf
//
//   [s] SCSI Commands Reference Manual 100293068, Rev. J
//   https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification. References are prefixed by
// the letter in brackets as listed above.
#pragma once

#include <cstdint>

#include "atapi_defs.h"

namespace vixen {
namespace hw {
namespace atapi {

// ----- Packet Command State -------------------------------------------------

// Keeps the state of a packet command execution
struct PacketCommandState {
    struct InputParameters {
        // Whether the operation is to be overlapped
        bool overlapped;

        // Whether to execute a DMA (true) or PIO (false) transfer
        bool dmaTransfer;

        // The operation tag
        uint8_t tag;

        // The maximum number of bytes to transfer in a single DRQ block
        uint16_t byteCountLimit;

        // The selected device (DEV)
        uint8_t selectedDevice;
    } input;

    // The command descriptor block
    CommandDescriptorBlock cdb;

    struct DataBuffer {
        ~DataBuffer();

        // Allocates a buffer for data transfer
        bool Allocate(uint32_t size);

        // Copy data into the destination buffer, returning the number of bytes read
        uint32_t Read(void *dst, uint32_t length);

        // Copy data from the source buffer, returning the number of bytes written
        uint32_t Write(void *src, uint32_t length);

        // Clears the buffer, resetting the read and write pointers
        void Clear() { m_readPos = m_writePos = m_size = 0; }

        // Determines if the buffer's data has been fully read from
        bool IsReadFinished() { return m_readPos >= m_size; }

        // Determines if the buffer's data has been fully written to
        bool IsWriteFinished() { return m_writePos >= m_cap; }

    private:
        // The data buffer to be used with transfer operations
        uint8_t *m_buf = nullptr;

        // The size of the data buffer, i.e. the number of valid bytes written to the buffer
        uint32_t m_size;

        // The capacity of the data buffer, i.e. the length of the buffer
        uint32_t m_cap;

        // The position of the writer
        uint32_t m_writePos;

        // The position of the reader
        uint32_t m_readPos;
    } dataBuffer;

    // Execution result data, filled in by the driver once the command is processed
    struct ExecutionResult {
        // The status code of the packet command execution.
        StatusCode status = StGood;

        // The sense key for the error encountered while processing the packet command.
        // Only valid if status is not StGood.
        SenseKey senseKey = SKNoSense;

        // The additional sense code and qualifier.
        // Only valid if status is not StGood and sense key is not No Sense.
        AdditionalSenseCode additionalSenseCode = ASCNone;

        // Whether the command was aborted because of an invalid operation code or
        // parameter, or because the device cannot complete the operation.
        bool aborted = false;

        // Indicates that the data transfer has reached the end of the medium.
        bool endOfMedium = false;

        // Indicates that the transfer length of a data transfer was incorrect.
        bool incorrectLength = false;

        // Indicates that the device driver encountered a problem executing the command.
        bool deviceFault = false;
    } result;
};

}
}
}
