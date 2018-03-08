#pragma once

#include <cstdint>

namespace openxbox {

/*!
 * Maps I/O and MMIO reads and writes to the corresponding devices.
 */
class IOMapper {
public:
    virtual void IORead(uint32_t addr, uint32_t *value, uint16_t size) = 0;
    virtual void IOWrite(uint32_t addr, uint32_t value, uint16_t size) = 0;

    virtual void MMIORead(uint32_t addr, uint32_t *value, uint8_t size) = 0;
    virtual void MMIOWrite(uint32_t addr, uint32_t value, uint8_t size) = 0;
};


}
