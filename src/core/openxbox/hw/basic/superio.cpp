#include "superio.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {

SuperIO::SuperIO() {
}

void SuperIO::Reset() {
}

bool SuperIO::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    log_warning("SuperIO::IORead:  Unhandled read!   port = 0x%x,  size = %d\n", port, size);
    return false;
}

bool SuperIO::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    log_warning("SuperIO::IOWrite: Unhandled write!  port = 0x%x,  size = %d,  value = 0x%x\n", port, size, value);
    return false;
}

}
