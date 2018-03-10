#include "defs.h"
#include "lpc.h"
#include "openxbox/log.h"

namespace openxbox {

LPCDevice::LPCDevice() {
}

// PCI Device functions

void LPCDevice::Init() {
    WriteConfigRegister8(PCI_CONFIG_HEADER_TYPE, PCI_TYPE_DEVICE);
    WriteConfigRegister16(PCI_CONFIG_VENDOR_ID, PCI_VENDOR_ID_NVIDIA);
    WriteConfigRegister16(PCI_CONFIG_DEVICE_ID, 0x01B2);

    RegisterBAR(0, 0x100); // I/O: 0x8000 - 0x80FF
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
