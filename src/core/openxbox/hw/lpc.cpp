#include "defs.h"
#include "lpc.h"
#include "openxbox/log.h"

namespace openxbox {

LPCDevice::LPCDevice() {
}

// PCI Device functions

void LPCDevice::Init() {
    RegisterBAR(0, 0x100, 0x8000);

    m_deviceID = 0x01B2;
    m_vendorID = PCI_VENDOR_ID_NVIDIA;
}

void LPCDevice::Reset() {
}

uint32_t LPCDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    log_spew("LPCDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    // TODO
    
    return 0;
}

void LPCDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    log_spew("LPCDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
}

uint32_t LPCDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    log_spew("LPCDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO

    return 0;
}

void LPCDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    log_spew("LPCDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
}

}
