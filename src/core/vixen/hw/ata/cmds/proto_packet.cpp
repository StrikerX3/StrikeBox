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

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

// Notes regarding the protocol:
// - There are two fluxograms: PIO/non-data transfers and DMA transfers
//   - They're largely the same, except for the actual data transfer and some register flag manipulations
// 
// - The Sector Count register is called the Interrupt register
// - The Cylinder Low/High registers are called Byte Count Low/High registers
// - The Features register contains the following fields:
//     bit 1     (OVL)  Indicates an overlapped command
//     bit 0     (DMA)  Use DMA for data transfer (not for the Packet command itself)
// - The Interrupt register contains the following fields:
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
    , m_packetBuffer(nullptr)
{
}

PacketProtocolCommand::~PacketProtocolCommand() {
    if (m_packetBuffer != nullptr) {
        delete[] m_packetBuffer;
    }
}

void PacketProtocolCommand::Execute() {
    // Read input according to the protocol [8.21.4]
    m_overlapped = !!(m_regs.features & PkFeatOverlapped);
    m_dmaTransfer = !!(m_regs.features & PkFeatDMATransfer);
    m_tag = (m_regs.sectorCount >> kPkTagShift) & kPkTagMask;
    m_byteCount = m_regs.cylinder;
    if (m_byteCount == 0xFFFF) {
        m_byteCount = 0xFFFE;
    }
    m_selectedDevice = m_regs.GetSelectedDeviceIndex();

    bool ready = PrepareToAcceptPacket();
    if (ready) {
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
        m_packetBuffer = new uint8_t[m_driver->GetPacketTransferSize()];
        m_packetPos = 0;

        // Follow (A) in the protocol fluxogram
    }
    else {
        HandleProtocolTail(true);
    }
}

void PacketProtocolCommand::ReadData(uint8_t *value, uint32_t size) {
    log_debug("PacketProtocolCommand::ReadData:  interrupt = 0x%x  status = 0x%x  size = %d\n", m_regs.sectorCount, m_regs.status, size);
}

void PacketProtocolCommand::WriteData(uint8_t *value, uint32_t size) {
    // Determine if the host is writing the Packet command itself or the data it requested
    if (m_regs.sectorCount & PkIntrCmdOrData) {
        // Writing the Packet command itself
        memcpy(m_packetBuffer + m_packetPos, value, size);
        m_packetPos += size;
        
        // Done writing the Packet command?
        if (m_packetPos >= m_driver->GetPacketTransferSize()) {
            ProcessPacket();
        }
    }
    else {
        // Writing the data requested by the Packet command
        log_debug("PacketProtocolCommand::WriteData:  interrupt = 0x%x  status = 0x%x  size = %d\n", m_regs.sectorCount, m_regs.status, size);
    }
}

bool PacketProtocolCommand::PrepareToAcceptPacket() {
    // TODO: if the packet command expects data:
    // - byteCount == 0 -> set ABRT and stop command
    // - (byteCount & 1) == 0 -> total requested data transfer length > byteCount
    // - (byteCount & 1) == 1 -> total requested data transfer length <= byteCount  (maybe)
    return true;
}

void PacketProtocolCommand::ProcessPacket() {
    log_debug("PacketProtocolCommand::ProcessPacket:  Processing packet\n");
    m_regs.status |= StBusy;
    m_regs.status &= ~StDataRequest;

    // TODO: Begin command execution
    bool succeeded = true;//command.Begin();
    
    // Handle error
    // TODO: what could cause an error here?
    if (!succeeded) {
        HandleProtocolTail(true);
        return;
    }

    // TODO: determine if the packet command needs data, and what direction it needs to go
    bool cmdRequiresData = false; // TODO: inline: command.RequiresData();
    if (cmdRequiresData) {
        // Check for overlapped execution (corresponds to (B) in the protocol fluxogram)
        if (m_driver->SupportsOverlap() && m_driver->IsOverlapEnabled() && m_overlapped) {
            ProcessPacketOverlapped();
        }
        else {
            ProcessPacketImmediate();
        }
    }
    else {
        // TODO: Execute non-data command
        bool succeeded = true; // command.Execute();
        HandleProtocolTail(!succeeded);
    }
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
        m_regs.cylinder = 0; // TODO: get from packet command
    }

    // Set I/O
    bool isWrite = true; // TODO: get from packet command
    if (isWrite) {
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
    Finish();
}

void PacketProtocolCommand::ProcessPacketOverlapped() {
    // TODO: implement (F)
    log_warning("PacketProtocolCommand::ProcessPacketOverlapped:  Unimplemented!\n");
}

void PacketProtocolCommand::HandleProtocolTail(bool hasError) {
    if (hasError) {
        m_regs.status |= StError;
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
