#pragma once

#include <cstdint>

#include "openxbox/cpu.h"
#include "serial.h"

namespace openxbox {

#define PORT_SUPERIO_CONFIG      0x2E
#define PORT_SUPERIO_DATA        0x2F

#define PORT_SUPERIO_BASE        PORT_SUPERIO_CONFIG
#define PORT_SUPERIO_COUNT       2

#define SUPERIO_SERIAL_PORT_COUNT 2


#define DEVICE_FDD               0x0
#define DEVICE_PARALLEL_PORT     0x3
#define DEVICE_SERIAL_PORT_1     0x4
#define DEVICE_SERIAL_PORT_2     0x5
#define DEVICE_KEYBOARD          0x7
#define DEVICE_GAME_PORT         0x9
#define DEVICE_PME               0xA
#define DEVICE_MPU_401           0xB
#define MAX_DEVICE               0xC

#define ENTER_CONFIG_KEY         0x55
#define EXIT_CONFIG_KEY          0xAA

#define MAX_CONFIG_REG           0x30
#define MAX_DEVICE_REGS          0xFF

#define CONFIG_DEVICE_NUMBER     0x07
#define CONFIG_PORT_LOW          0x26
#define CONFIG_PORT_HIGH         0x27

#define CONFIG_DEVICE_ACTIVATE              0x30
#define CONFIG_DEVICE_BASE_ADDRESS_HIGH     0x60
#define CONFIG_DEVICE_BASE_ADDRESS_LOW      0x61
#define CONFIG_DEVICE_INTERRUPT             0x70

class SuperIO : public IODevice {
public:
    SuperIO(IRQHandler *irqHandler, CharDriver *chrs[SUPERIO_SERIAL_PORT_COUNT]);
    virtual ~SuperIO();

    void Init();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;

private:
    void UpdateDevices();

    bool m_inConfigMode;
    uint32_t m_selectedReg;

    uint8_t m_configRegs[MAX_CONFIG_REG];
    uint8_t m_deviceRegs[MAX_DEVICE][MAX_DEVICE_REGS];

    Serial *m_serialPorts[SUPERIO_SERIAL_PORT_COUNT];
};

}
