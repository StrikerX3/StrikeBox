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
#include "vixen/hw/atapi/cmds/cmd_mode_sense_10.h"
#include "vixen/hw/atapi/cmds/cmd_read_10.h"
#include "vixen/hw/atapi/cmds/cmd_read_capacity.h"
#include "vixen/hw/atapi/cmds/cmd_read_dvd_structure.h"
#include "vixen/hw/atapi/cmds/cmd_test_unit_ready.h"

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

    // Corresponds to the end of the protocol fluxogram starting from (C) when
    // hasError is true, or (D) if false.
    void HandleProtocolTail(bool hasError);

    // Processes the packet received from the host.
    void ProcessPacket();

    // Prepares registers for data transfer.
    // Corresponds to the protocol fluxogram starting from (B).
    void PrepareDataTransfer();

    // Prepares registers for an immediate (non-overlapped) data transfer.
    void ProcessPacketImmediate();

    // Prepares registers for an overlapped data transfer.
    void ProcessPacketOverlapped();
    
    // ----- State ------------------------------------------------------------

    uint8_t *m_packetCmdBuffer;
    uint8_t m_packetCmdPos;

    atapi::PacketCommandState m_packetCmdState;

    atapi::cmd::IATAPICommand *m_command;
};

// Map commands to their factories
const std::unordered_map<uint8_t, atapi::cmd::IATAPICommand::Factory, std::hash<uint8_t>> kCmdFactories = {
    { atapi::OpModeSense10, atapi::cmd::ModeSense10::Factory },
    { atapi::OpRead10, atapi::cmd::Read10::Factory },
    { atapi::OpReadCapacity, atapi::cmd::ReadCapacity::Factory },
    { atapi::OpReadDVDStructure, atapi::cmd::ReadDVDStructure::Factory },
    //{ atapi::OpRequestSense, atapi::cmd::RequestSense::Factory },
    { atapi::OpTestUnitReady, atapi::cmd::TestUnitReady::Factory },
};

}
}
}
}
