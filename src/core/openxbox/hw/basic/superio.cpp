#include "superio.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {

SuperIO::SuperIO() {
    memset(m_configRegs, 0, sizeof(m_configRegs));
    memset(m_deviceRegs, 0, sizeof(m_deviceRegs));

    m_configRegs[CONFIG_PORT_LOW] = (uint8_t)(PORT_SUPERIO_BASE & 0xFF);
    m_configRegs[CONFIG_PORT_HIGH] = (uint8_t)(PORT_SUPERIO_BASE >> 8);

    m_inConfigMode = false;
    m_selectedReg = 0;

    // TODO: init serial cores
}

void SuperIO::Reset() {
}

bool SuperIO::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(PORT_SUPERIO_BASE, PORT_SUPERIO_COUNT, this)) return false;
    if (!mapper->MapIODevice(PORT_SUPERIO_UART_BASE_1, PORT_SUPERIO_UART_COUNT_1, this)) return false;
    if (!mapper->MapIODevice(PORT_SUPERIO_UART_BASE_2, PORT_SUPERIO_UART_COUNT_2, this)) return false;

    return true;
}

void SuperIO::UpdateDevices() {
    // TODO: update serial cores
}

bool SuperIO::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("SuperIO::IORead:  port = 0x%x,  size = %d\n", port, size);

    switch (port) {
    case PORT_SUPERIO_CONFIG:
        *value = 0;
        return true;
    case PORT_SUPERIO_DATA:
        if (m_selectedReg < MAX_CONFIG_REG) {
            *value = m_configRegs[m_selectedReg];
        }
        else {
            if (m_configRegs[CONFIG_DEVICE_NUMBER] >= MAX_DEVICE) {
                log_warning("SuperIO::IORead:  Device number out of range!  %d >= %d\n", m_configRegs[CONFIG_DEVICE_NUMBER], MAX_DEVICE);
                *value = 0;
            }
            else {
                uint8_t* dev = m_deviceRegs[m_configRegs[CONFIG_DEVICE_NUMBER]];
                *value = dev[m_selectedReg];
            }
        }
        return true;
    }

    if (port >= PORT_SUPERIO_UART_BASE_1 && port <= PORT_SUPERIO_UART_END_1) {
        // TODO: redirect to serial device 1
    }

    if (port >= PORT_SUPERIO_UART_BASE_2 && port <= PORT_SUPERIO_UART_END_2) {
        // TODO: redirect to serial device 2
    }

    log_warning("SuperIO::IORead:  Unhandled read!   port = 0x%x,  size = %d\n", port, size);
    return false;
}

bool SuperIO::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    log_spew("SuperIO::IOWrite: port = 0x%x,  size = %d,  value = 0x%x\n", port, size, value);

    switch (port) {
    case PORT_SUPERIO_CONFIG:
        if (value == ENTER_CONFIG_KEY) {
#ifdef _DEBUG
            if (m_inConfigMode) {
                log_warning("SuperIO::IOWrite: Attempted to reenter configuration mode\n");
            }
            log_debug("SuperIO::IOWrite: Entering configuration mode\n");
#endif
            m_inConfigMode = true;
        }
        else if (value == EXIT_CONFIG_KEY) {
#ifdef _DEBUG
            if (!m_inConfigMode) {
                log_warning("SuperIO::IOWrite: Attempted to reexit configuration mode\n");
            }
            log_debug("SuperIO::IOWrite: Exiting configuration mode\n");
#endif
            m_inConfigMode = false;

            UpdateDevices();
        }
        else {
            m_selectedReg = value;
        }
        return true;
    case PORT_SUPERIO_DATA:
        if (m_selectedReg < MAX_CONFIG_REG) {
            // Global configuration register
            m_configRegs[m_selectedReg] = value;
        }
        else {
            // Device register
#ifdef _DEBUG
            if (m_configRegs[CONFIG_DEVICE_NUMBER] >= MAX_DEVICE) {
                log_warning("SuperIO::IOWrite: Device number out of range!  %d >= %d\n", m_configRegs[CONFIG_DEVICE_NUMBER], MAX_DEVICE);
            }
            else
#endif
            {
                uint8_t* dev = m_deviceRegs[m_configRegs[CONFIG_DEVICE_NUMBER]];
                dev[m_selectedReg] = value;
            }
        }
        return true;
    }

    log_warning("SuperIO::IOWrite: Unhandled write!  port = 0x%x,  size = %d,  value = 0x%x\n", port, size, value);
    return false;
}

}
