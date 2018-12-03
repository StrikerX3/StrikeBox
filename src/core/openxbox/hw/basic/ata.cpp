#include "ata.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {

ATA::ATA(i8259 *pic)
    : m_pic(pic)
{
}

ATA::~ATA() {
}

void ATA::Reset() {
}

bool ATA::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(PORT_PRIMARY_ATA_COMMAND_BASE, PORT_PRIMARY_ATA_COMMAND_COUNT, this)) return false;
    if (!mapper->MapIODevice(PORT_PRIMARY_ATA_CONTROL, 1, this)) return false;

    if (!mapper->MapIODevice(PORT_SECONDARY_ATA_COMMAND_BASE, PORT_SECONDARY_ATA_COMMAND_COUNT, this)) return false;
    if (!mapper->MapIODevice(PORT_SECONDARY_ATA_CONTROL, 1, this)) return false;

    return true;
}

bool ATA::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    log_warning("ATA::IORead:  Unhandled read!   port = 0x%x,  size = %u\n", port, size);

    *value = 0;

    return false;
}

bool ATA::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    log_warning("ATA::IOWrite: Unhandled write!  port = 0x%x,  size = %u,  value = 0x%x\n", port, size, value);
    return false;
}

}
