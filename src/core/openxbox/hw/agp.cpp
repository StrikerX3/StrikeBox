#include "defs.h"
#include "agp.h"
#include "openxbox/log.h"

namespace openxbox {

AGPDevice::AGPDevice() {
}

// PCI Device functions

void AGPDevice::Init() {
    RegisterBAR(0, 0x100, 0x8000);

    m_deviceID = 0x01B7;
    m_vendorID = PCI_VENDOR_ID_NVIDIA;
}

void AGPDevice::Reset() {
}

uint32_t AGPDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("AGPDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO

    return 0;
}

void AGPDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("AGPDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t AGPDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("AGPDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void AGPDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("AGPDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
