#pragma once

#include <cstdint>

namespace openxbox {

#define PCI_DEVFN(slot, func) ((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_DEVID(bus, devfn)  ((((uint16_t)(bus)) << 8) | (devfn))

typedef struct {
    uint8_t registerNumber : 8;
    uint8_t functionNumber : 3;
    uint8_t deviceNumber : 5;
    uint8_t busNumber : 8;
    uint8_t reserved : 7;
    uint8_t enable : 1;
} PCIConfigAddressRegister;

}
