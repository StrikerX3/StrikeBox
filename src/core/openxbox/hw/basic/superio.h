#pragma once

#include <cstdint>

#include "openxbox/cpu.h"

namespace openxbox {

#define PORT_SUPERIO_CONFIG  0x2E
#define PORT_SUPERIO_DATA    0x2F

#define PORT_SUPERIO_BASE    PORT_SUPERIO_CONFIG
#define PORT_SUPERIO_COUNT   2

#define PORT_SUPERIO_UART_BASE   0x3F8
#define PORT_SUPERIO_UART_COUNT  8

class SuperIO : public IODevice {
public:
    SuperIO();
	void Reset();

	bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;
private:
};

}
