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
#include "vixen/hw/ata/cmds/ata_command.h"

#include "vixen/log.h"

namespace vixen {
namespace hw {
namespace ata {
namespace cmd {

IATACommand::IATACommand(ATADevice& device)
    : m_device(device)
    , m_regs(device.GetRegisters())
    , m_driver(device.GetDriver())
    , m_channel(device.GetChannel())
    , m_devIndex(device.GetIndex())
    , m_interrupt(device.GetInterrupt())
    , m_finished(false)
{
}

IATACommand::~IATACommand() {
}

}
}
}
}
