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

#include "ata_command.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

/*!
 * Base class for all commands based on the PACKET protocol [9.11].
 */
class PacketProtocolCommand : public IATACommand {
public:
    PacketProtocolCommand(ATADevice& device);
    virtual ~PacketProtocolCommand() override;

    // ----- Low-level operations ---------------------------------------------

    void Execute() override;
    void ReadData(uint8_t *value, uint32_t size) override;
    void WriteData(uint8_t *value, uint32_t size) override;

private:
    // ----- Protocol operations ----------------------------------------------

    // Parses the input registers and stores the values in the parameters.
    bool ReadInput();

    // Corresponds to the end of the protocol fluxogram starting from (C) when
    // hasError is true, or (D) if false.
    void HandleProtocolTail(bool hasError);

    // Processes the packet received from the host.
    void ProcessPacket();

    // Prepares registers for data transfer.
    // Corresponds to the protocol fluxogram starting from (B).
    void PrepareDataTransfer();

    void ProcessPacketImmediate();
    void ProcessPacketOverlapped();

    // ----- Parameters -------------------------------------------------------

    bool m_overlapped;
    bool m_dmaTransfer;
    uint8_t m_tag;
    uint16_t m_byteCountLimit;
    uint8_t m_selectedDevice;
    
    // ----- State ------------------------------------------------------------

    uint8_t *m_packetCmdBuffer;
    uint8_t m_packetCmdPos;

    uint8_t *m_packetDataBuffer;
    uint16_t m_packetDataPos;
    uint32_t m_packetDataTotal;
    uint32_t m_packetDataSize;

    atapi::PacketInformation m_packetInfo;
};

}
}
}
}
