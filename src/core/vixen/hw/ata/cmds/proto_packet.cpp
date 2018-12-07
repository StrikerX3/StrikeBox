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
#include "proto_packet.h"

#include "vixen/log.h"
#include "../atapi_defs.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

// Notes regarding the protocol:
// - There are two fluxograms: PIO/non-data transfers and DMA transfers
//   - They're largely the same, except for the actual data transfer and some register flag manipulations
// 
// - The Sector Count register is called the Interrupt Reason register
// - The Cylinder Low/High registers are called Byte Count Low/High registers
// - The Features register contains the following fields:
//     bit 1     (OVL)  Indicates an overlapped command
//     bit 0     (DMA)  Use DMA for data transfer (not for the Packet command itself)
// - The Interrupt Reason register contains the following fields:
//     bit 7..3  (Tag)  (Overlapped only) Command tag
//     bit 2     (REL)  (Overlapped only) Indicates that the device is performing a bus release
//     bit 1     (I/O)  When set to 1, indicates a transfer to the host; 0 indicates transfer to the device
//     bit 0     (C/D)  When set to 1, indicates a command packet; 0 indicates a data packet
// - The Status register is largely the same, except for the following fields:
//     bit 5     (DMRD) DMA ready
//     bit 4     (SERV) (Overlapped only) Indicates that another command can be serviced
//     bit 0     (CHK)  Indicates an error; host should check the Error register sense key or code bit
//
// Overlapped commands allow the device to execute a long-running operation in the background
// while still accepting new packet commands.

PacketProtocolCommand::PacketProtocolCommand(ATADevice& device)
    : IATACommand(device)
    , m_packetCmdBuffer(nullptr)
    , m_packetDataBuffer(nullptr)
{
}

PacketProtocolCommand::~PacketProtocolCommand() {
    if (m_packetCmdBuffer != nullptr) {
        delete[] m_packetCmdBuffer;
    }
    if (m_packetDataBuffer != nullptr) {
        delete[] m_packetDataBuffer;
    }
}

void PacketProtocolCommand::Execute() {
    if (ReadInput()) {
        // Update Interrupt register
        m_regs.sectorCount |= PkIntrCmdOrData;
        m_regs.sectorCount &= ~PkIntrIODirection;
        // On PIO and non-data transfers, Bus Release is cleared
        if (!m_dmaTransfer) {
            m_regs.sectorCount &= ~PkIntrBusRelease;
        }

        // Update Status register
        m_regs.status |= StDataRequest;
        m_regs.status &= ~StBusy;

        // Allocate buffer for the packet
        m_packetCmdBuffer = new uint8_t[m_driver->GetPacketTransferSize()];
        m_packetCmdPos = 0;

        // Follow (A) in the protocol fluxogram
    }
    else {
        HandleProtocolTail(true);
    }
}

bool PacketProtocolCommand::ReadInput() {
    // Read input according to the protocol [8.21.4]
    m_overlapped = !!(m_regs.features & PkFeatOverlapped);
    m_dmaTransfer = !!(m_regs.features & PkFeatDMATransfer);
    m_tag = (m_regs.sectorCount >> kPkTagShift) & kPkTagMask;
    m_byteCountLimit = m_regs.cylinder;
    m_selectedDevice = m_regs.GetSelectedDeviceIndex();

    return true;
}

void PacketProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    // Host is reading the data requested by the Packet command
   
    // If the packet data buffer is empty, ask driver to fill in a full block
    // of data (up to the byte count limit) and tell us how many bytes it filled
    if (m_packetDataSize == 0) {
        // Let the driver fill in the data buffer
        if (!m_driver->ProcessATAPIPacketDataRead(m_packetInfo, m_packetDataBuffer, m_byteCountLimit, &m_packetDataSize)) {
            m_transferError = true;
            return;
        }
    }
    
    // Copy from buffer to value
    memcpy(value, m_packetDataBuffer + m_packetDataPos, size);
    m_packetDataPos += size;

    // Done reading the packet data?
    if (m_packetDataPos >= m_packetDataSize) {
        m_regs.status |= StBusy;
        m_regs.status &= ~StDataRequest;

        // Done transferring all the data needed by the packet?
        m_packetDataTotal += m_packetDataPos;
        if (m_packetDataTotal >= m_packetInfo.transferSize) {
            HandleProtocolTail(m_transferError);
            return;
        }

        // Let the driver fill in the data buffer again
        if (!m_driver->ProcessATAPIPacketDataRead(m_packetInfo, m_packetDataBuffer, m_byteCountLimit, &m_packetDataSize)) {
            m_transferError = true;
            return;
        }

        m_packetDataPos = 0;
    }
}

void PacketProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Determine if the host is writing the Packet command itself or the data it requested
    if (m_regs.sectorCount & PkIntrCmdOrData) {
        // Writing the Packet command itself
        memcpy(m_packetCmdBuffer + m_packetCmdPos, value, size);
        m_packetCmdPos += size;

        // Done writing the Packet command?
        if (m_packetCmdPos >= m_driver->GetPacketTransferSize()) {
            ProcessPacket();
        }
    }
    else {
        // Writing the data requested by the Packet command
        memcpy(m_packetDataBuffer + m_packetDataPos, value, size);
        m_packetDataPos += size;

        // Done writing the packet data?
        if (m_packetDataPos >= m_byteCountLimit) {
            m_regs.status |= StBusy;
            m_regs.status &= ~StDataRequest;
            
            // Let the driver process the data
            if (!m_driver->ProcessATAPIPacketDataWrite(m_packetInfo, m_packetDataBuffer, m_byteCountLimit)) {
                m_transferError = true;
                return;
            }

            // Done transferring all the data needed by the packet?
            m_packetDataTotal += m_packetDataPos;
            if (m_packetDataTotal >= m_packetInfo.transferSize) {
                HandleProtocolTail(m_transferError);
                return;
            }

            m_packetDataPos = 0;
        }
    }
}

void PacketProtocolCommand::ProcessPacket() {
    log_debug("PacketProtocolCommand::ProcessPacket:  Processing packet\n");
    m_regs.status |= StBusy;
    m_regs.status &= ~StDataRequest;

    // Identify packet and check if the device can process it
    bool succeeded = m_driver->IdentifyATAPIPacket(m_packetCmdBuffer, m_packetInfo);

    // Handle error
    if (!succeeded) {
        HandleProtocolTail(true);
        return;
    }

    // If the byte count limit is zero, set ABRT and stop command
    if (m_byteCountLimit == 0) {
        m_regs.error |= ErrAbort;
        HandleProtocolTail(true);
        return;
    }

    // If the total requested data transfer length is greater than the byte count limit, then the byte count limit must be even
    // If the total requested data transfer length is less than or equal to the byte count limit, then the byte count limit could be even or odd
    if (m_packetInfo.transferSize > m_byteCountLimit && (m_byteCountLimit & 1)) {
        m_regs.error |= ErrAbort;
        HandleProtocolTail(true);
        return;
    }

    // A byte count limit of 0xFFFF is interpreted by the device as though it were 0xFFFE
    if (m_byteCountLimit == 0xFFFF) {
        m_byteCountLimit = 0xFFFE;
    }

    if (m_packetInfo.operationType == atapi::PktOpNonData) {
        // Execute non-data command
        bool succeeded = m_driver->ProcessATAPIPacketNonData(m_packetInfo);
        HandleProtocolTail(!succeeded);
    }
    else {
        PrepareDataTransfer();
    }
}

void PacketProtocolCommand::PrepareDataTransfer() {
    // Check for overlapped execution (corresponds to (B) in the protocol fluxogram)
    if (m_driver->SupportsOverlap() && m_driver->IsOverlapEnabled() && m_overlapped) {
        ProcessPacketOverlapped();
    }
    else {
        ProcessPacketImmediate();
    }

    m_transferError = false;
}

void PacketProtocolCommand::ProcessPacketImmediate() {
    if (m_dmaTransfer) {
        m_regs.sectorCount &= ~PkIntrBusRelease;
        m_regs.status &= ~StService;
        m_regs.status |= StDMAReady;
    }
    else {
        // Set Tag
        m_regs.sectorCount &= ~(kPkTagMask << kPkTagShift);
        m_regs.sectorCount |= m_tag << kPkTagShift;

        // Set byte count
        m_regs.cylinder = m_packetInfo.transferSize;
    }

    // Set I/O
    if (m_packetInfo.operationType == atapi::PktOpDataOut) {
        m_regs.sectorCount |= PkIntrIODirection;
    }
    else {
        m_regs.sectorCount &= ~PkIntrIODirection;
    }

    // Clear C/D=0
    m_regs.sectorCount &= ~PkIntrCmdOrData;

    // Update Status register
    m_regs.status |= StDataRequest;
    m_regs.status &= ~StBusy;

    // Allocate buffer for the data transfer
    m_packetDataBuffer = new uint8_t[m_byteCountLimit];
    m_packetDataPos = 0;
    m_packetDataTotal = 0;
    m_packetDataSize = 0;

    m_interrupt.Assert();
    Finish();
}

void PacketProtocolCommand::ProcessPacketOverlapped() {
    // TODO: implement (F)
    log_warning("PacketProtocolCommand::ProcessPacketOverlapped:  Unimplemented!\n");
}

void PacketProtocolCommand::HandleProtocolTail(bool hasError) {
    if (hasError) {
        m_regs.status |= StError;

        // Fill in error output according to the protocol [8.21.6]
        
        // Error register:
        //  "Sense Key is a command packet set specific error indication."
        m_regs.error &= ~(kPkSenseMask << kPkSenseShift);
        m_regs.error |= (m_packetInfo.senseKey & kPkSenseMask) << kPkSenseShift;

        //  "ABRT shall be set to one if the requested command has been command
        //   [sic] aborted because the command code or a command parameter is
        //   invalid. ABRT may be set to one if the device is not able to
        //   complete the action requested by the command."
        if (m_packetInfo.aborted) {
            m_regs.error |= ErrAbort;
        }
        else {
            m_regs.error &= ~ErrAbort;
        }

        //  "EOM - the meaning of this bit is command set specific. See the
        //   appropriate command set standard for its definition."
        if (m_packetInfo.endOfMedium) {
            m_regs.error |= PkErrEndOfMedium;
        }
        else {
            m_regs.error &= ~PkErrEndOfMedium;
        }

        //  "ILI - the meaning of this bit is command set specific. See the
        //   appropriate command set standard for its definition."
        if (m_packetInfo.incorrectLengthIndicator) {
            m_regs.error |= PkErrIncorrectLengthIndicator;
        }
        else {
            m_regs.error &= ~PkErrIncorrectLengthIndicator;
        }
        
        // Interrupt reason register:
        //  "Tag - If the device supports command queuing and overlap is
        //   enabled, this field contains the command Tag for the command.
        //   If the device does not support command queuing or overlap is
        //   disabled, this field is not applicable."
        //     We'll fill it in regardless of command queuing or overlap support
        m_regs.sectorCount &= ~(kPkTagMask << kPkTagShift);
        m_regs.sectorCount |= m_tag << kPkTagShift;

        //  "REL - Shall be cleared to zero."        m_regs.sectorCount &= ~PkIntrBusRelease;
        
        //  "I/O - Shall be set to one."
        //  "C/D - Shall be set to one."
        //     Done below

        // Device/Head register:
        //  "DEV shall indicate the selected device."
        //     Not necessary, but the spec says so
        m_regs.deviceHead = (m_regs.deviceHead & ~(1 << kDevSelectorBit)) | (m_devIndex << kDevSelectorBit);

        // Status register:
        //  "BSY shall be cleared to zero indicating command completion."
        //  "DRDY shall be set to one."
        //  "DRQ shall be cleared to zero."
        //    Done below

        //  "SERV(Service) - Shall be set to one if another command is ready to be serviced.
        //   If overlap is not supported, this bit is command specific."
        // TODO: support overlapped operations

        //  "DF(Device Fault) shall be set to one if a device fault has occurred."
        if (m_packetInfo.deviceFault) {
            m_regs.status |= StDeviceFault;
        }

        //  "CHK shall be set to one if an Error register sense key or code bit is set."
        if (m_regs.error) {
            m_regs.status |= StCheck;
        }
    }

    // The Sector Count register is called the Interrupt register on the Packet protocol
    m_regs.sectorCount |= PkIntrIODirection | PkIntrCmdOrData;
    m_regs.sectorCount &= ~PkIntrBusRelease;

    if (m_dmaTransfer) {
        m_regs.status &= ~StDataRequest;
    }
    m_regs.status |= StReady;
    m_regs.status &= ~StBusy;
    m_interrupt.Assert();
    Finish();
}

}
}
}
}
