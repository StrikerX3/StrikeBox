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
#include "vixen/hw/atapi/atapi_defs.h"
#include "vixen/hw/atapi/atapi_xbox.h"
#include "vixen/hw/atapi/atapi_utils.h"
#include "vixen/hw/atapi/cmds/atapi_command.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

using namespace vixen::hw::atapi;

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
// - The Status register includes the following fields:
//     bit 5     (DMRD) DMA ready
//     bit 4     (SERV) (Overlapped only) Indicates that another command can be serviced
//     bit 0     (CHK)  Indicates an error; host should check the Error register sense key or code bit
//
// Overlapped commands allow the device to execute a long-running operation in the background
// while still accepting new packet commands.

PacketProtocolCommand::PacketProtocolCommand(ATADevice& device)
    : IATACommand(device)
    , m_packetCmdBuffer(nullptr)
    , m_command(nullptr)
{
}

PacketProtocolCommand::~PacketProtocolCommand() {
    if (m_packetCmdBuffer != nullptr) {
        delete[] m_packetCmdBuffer;
    }
    if (m_command != nullptr) {
        delete m_command;
    }
}

void PacketProtocolCommand::Execute() {
    // Read input according to the protocol [8.21.4]
    m_packetCmdState.input.overlapped = !!(m_regs.features & PkFeatOverlapped);
    m_packetCmdState.input.dmaTransfer = !!(m_regs.features & PkFeatDMATransfer);
    m_packetCmdState.input.tag = (m_regs.sectorCount >> kPkTagShift) & kPkTagMask;
    m_packetCmdState.input.byteCountLimit = m_regs.cylinder;
    m_packetCmdState.input.selectedDevice = m_regs.GetSelectedDeviceIndex();
    
    // If the byte count limit is zero, set ABRT and stop command
    if (m_packetCmdState.input.byteCountLimit == 0) {
        m_regs.error |= ErrAbort;
        HandleProtocolTail(true);
        return;
    }

    // A byte count limit of 0xFFFF is interpreted by the device as though it were 0xFFFE
    if (m_packetCmdState.input.byteCountLimit == 0xFFFF) {
        m_packetCmdState.input.byteCountLimit = 0xFFFE;
    }

    // Update Interrupt register
    m_regs.sectorCount |= PkIntrCmdOrData;
    m_regs.sectorCount &= ~PkIntrIODirection;
    // On PIO and non-data transfers, Bus Release is cleared
    if (!m_packetCmdState.input.dmaTransfer) {
        m_regs.sectorCount &= ~PkIntrBusRelease;
    }

    // Update Status register
    m_regs.status |= StDataRequest;
    m_regs.status &= ~StBusy;

    // Allocate buffer for the packet
    m_packetCmdBuffer = new uint8_t[m_driver->GetPacketCommandSize()];
    m_packetCmdPos = 0;

    // Follow (A) in the protocol fluxogram
}

void PacketProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    // Host is reading the data requested by the Packet command
   
    // Read from buffer
    uint32_t sizeRead = m_packetCmdState.dataBuffer.Read(value, size);

    // Done reading the packet data?
    if (m_packetCmdState.dataBuffer.IsReadFinished()) {
        m_regs.status |= StBusy;
        m_regs.status &= ~StDataRequest;

        // Done transferring all the data needed by the packet?
        if (m_command->IsTransferFinished()) {
            HandleProtocolTail(false);
            return;
        }

        // Read more data
        if (!m_command->Execute()) {
            HandleProtocolTail(true);
            return;
        }
    }
}

void PacketProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Determine if the host is writing the Packet command itself or the data it requested
    if (m_regs.sectorCount & PkIntrCmdOrData) {
        // Writing the Packet command itself
        memcpy(m_packetCmdBuffer + m_packetCmdPos, value, size);
        m_packetCmdPos += size;

        // Done writing the Packet command?
        if (m_packetCmdPos >= m_driver->GetPacketCommandSize()) {
            ProcessPacket();
        }
    }
    else {
        // Writing the data requested by the Packet command

        // Write to buffer
        uint32_t sizeWritten = m_packetCmdState.dataBuffer.Write(value, size);

        // Done writing the packet data?
        if (m_packetCmdState.dataBuffer.IsWriteFinished()) {
            m_regs.status |= StBusy;
            m_regs.status &= ~StDataRequest;

            // Execute command with the current buffer
            if (!m_command->Execute()) {
                HandleProtocolTail(true);
                return;
            }
            
            // Done transferring all the data needed by the packet?
            if (m_command->IsTransferFinished()) {
                HandleProtocolTail(false);
                return;
            }
        }
    }
}

void PacketProtocolCommand::ProcessPacket() {
    //log_spew("PacketProtocolCommand::ProcessPacket:  Processing packet\n");
    m_regs.status |= StBusy;
    m_regs.status &= ~StDataRequest;

    // Get the command descriptor block
    atapi::CommandDescriptorBlock *cdb = reinterpret_cast<atapi::CommandDescriptorBlock *>(m_packetCmdBuffer);

    // Get the command factory for the command's operation code
    if (kCmdFactories.count(cdb->opCode.u8) == 0) {
        log_warning("PacketProtocolCommand::ProcessPacket:  Unimplemented command 0x%x!\n", cdb->opCode.u8);
        HandleProtocolTail(true);
        return;
    }

    log_spew("PacketProtocolCommand::ProcessPacket:  Processing command 0x%x\n", cdb->opCode.u8);

    // Instantiate the command.
    // This will also allocate the data buffer if necessary.
    m_packetCmdState.cdb = *cdb;
    auto factory = kCmdFactories.at(cdb->opCode.u8);
    m_command = factory(m_packetCmdState, m_driver);

    // Validate parameters; return error immediately if invalid.
    // Will also initialize the data buffer if a transfer is required.
    if (!m_command->Prepare()) {
        HandleProtocolTail(true);
        return;
    }

    if (m_command->GetOperationType() == PktOpNonData) {
        // Execute non-data command immediately
        bool succeeded = m_command->Execute();
        HandleProtocolTail(!succeeded);
    }
    else {
        // Prepare registers for a data transfer (in or out)
        PrepareDataTransfer();
    }
}

void PacketProtocolCommand::PrepareDataTransfer() {
    // Check for overlapped execution (corresponds to (B) in the protocol fluxogram)
    if (m_driver->SupportsOverlap() && m_driver->IsOverlapEnabled() && m_packetCmdState.input.overlapped) {
        ProcessPacketOverlapped();
    }
    else {
        ProcessPacketImmediate();
    }
}

void PacketProtocolCommand::ProcessPacketImmediate() {
    if (m_packetCmdState.input.dmaTransfer) {
        m_regs.sectorCount &= ~PkIntrBusRelease;
        m_regs.status &= ~StService;
        m_regs.status |= StDMAReady;
    }
    else {
        // Set Tag
        m_regs.sectorCount &= ~(kPkTagMask << kPkTagShift);
        m_regs.sectorCount |= m_packetCmdState.input.tag << kPkTagShift;

        // Set byte count
        m_regs.cylinder = m_packetCmdState.input.byteCountLimit;
    }

    // Set I/O
    if (m_command->GetOperationType() == atapi::PktOpDataOut) {
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

    m_interrupt.Assert();
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
        m_regs.error |= (m_packetCmdState.result.senseKey & kPkSenseMask) << kPkSenseShift;

        //  "ABRT shall be set to one if the requested command has been command
        //   [sic] aborted because the command code or a command parameter is
        //   invalid. ABRT may be set to one if the device is not able to
        //   complete the action requested by the command."
        if (m_packetCmdState.result.aborted) {
            m_regs.error |= ErrAbort;
        }
        else {
            m_regs.error &= ~ErrAbort;
        }

        //  "EOM - the meaning of this bit is command set specific. See the
        //   appropriate command set standard for its definition."
        if (m_packetCmdState.result.endOfMedium) {
            m_regs.error |= PkErrEndOfMedium;
        }
        else {
            m_regs.error &= ~PkErrEndOfMedium;
        }

        //  "ILI - the meaning of this bit is command set specific. See the
        //   appropriate command set standard for its definition."
        if (m_packetCmdState.result.incorrectLength) {
            m_regs.error |= PkErrIncorrectLength;
        }
        else {
            m_regs.error &= ~PkErrIncorrectLength;
        }
        
        // Interrupt reason register:
        //  "Tag - If the device supports command queuing and overlap is
        //   enabled, this field contains the command Tag for the command.
        //   If the device does not support command queuing or overlap is
        //   disabled, this field is not applicable."
        //     We'll fill it in regardless of command queuing or overlap support
        m_regs.sectorCount &= ~(kPkTagMask << kPkTagShift);
        m_regs.sectorCount |= m_packetCmdState.input.tag << kPkTagShift;

        //  "REL - Shall be cleared to zero."
        m_regs.sectorCount &= ~PkIntrBusRelease;
        
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
        if (m_packetCmdState.result.deviceFault) {
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

    if (m_packetCmdState.input.dmaTransfer) {
        m_regs.status &= ~StDataRequest;
    }
    m_regs.status |= StReady;
    m_regs.status &= ~StBusy;
    m_interrupt.Assert();
    log_spew("PacketProtocolCommand::HandleProtocolTail:  Packet command finished\n");
    Finish();
}

}
}
}
}
