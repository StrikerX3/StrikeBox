/*
 * Portions of the code are based on XQEMU's SMSC LPC47M157 (Super I/O) emulation.
 * The original copyright header is included below.
 */
/*
 * QEMU SMSC LPC47M157 (Super I/O)
 *
 * Copyright (c) 2013 espes
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include "strikebox/hw/basic/superio.h"

#include "strikebox/log.h"
#include "strikebox/io.h"

namespace strikebox {

const static uint32_t kSerialPortIOBases[] = {
    PORT_SERIAL_BASE_1,
    PORT_SERIAL_BASE_2
};

SuperIO::SuperIO(IRQHandler& irqHandler, CharDriver *chrs[SUPERIO_SERIAL_PORT_COUNT]) {
    memset(m_configRegs, 0, sizeof(m_configRegs));
    memset(m_deviceRegs, 0, sizeof(m_deviceRegs));

    m_configRegs[CONFIG_PORT_LOW] = (uint8_t)(PORT_SUPERIO_BASE & 0xFF);
    m_configRegs[CONFIG_PORT_HIGH] = (uint8_t)(PORT_SUPERIO_BASE >> 8);

    m_inConfigMode = false;
    m_selectedReg = 0;

    // Initialize serial ports
    for (int i = 0; i < SUPERIO_SERIAL_PORT_COUNT; i++) {
        m_serialPorts[i] = new Serial(irqHandler, kSerialPortIOBases[i]);
        m_serialPorts[i]->Init(chrs[i]);
        m_serialPorts[i]->SetBaudBase(115200);
    }
}

SuperIO::~SuperIO() {
    // Shutdown serial ports
    for (int i = 0; i < SUPERIO_SERIAL_PORT_COUNT; i++) {
        m_serialPorts[i]->Stop();
        delete m_serialPorts[i];
    }
}

void SuperIO::Init() {
}

void SuperIO::Reset() {
}

bool SuperIO::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(PORT_SUPERIO_BASE, PORT_SUPERIO_COUNT, this)) return false;
    for (int i = 0; i < SUPERIO_SERIAL_PORT_COUNT; i++) {
        if (!m_serialPorts[i]->MapIO(mapper)) return false;
    }

    return true;
}

void SuperIO::UpdateDevices() {
    for (int i = 0; i < SUPERIO_SERIAL_PORT_COUNT; i++) {
        uint8_t *dev = m_deviceRegs[DEVICE_SERIAL_PORT_1 + i];
        if (dev[CONFIG_DEVICE_ACTIVATE] && !m_serialPorts[i]->m_active) {
            //uint32_t iobase = (dev[CONFIG_DEVICE_BASE_ADDRESS_HIGH] << 8) | dev[CONFIG_DEVICE_BASE_ADDRESS_LOW];
            uint32_t irq = dev[CONFIG_DEVICE_INTERRUPT];

            m_serialPorts[i]->SetIRQ(irq);
            m_serialPorts[i]->m_active = true;
        }
    }
}

bool SuperIO::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("SuperIO::IORead:  port = 0x%x,  size = %u\n", port, size);

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

    log_warning("SuperIO::IORead:  Unhandled read!   port = 0x%x,  size = %u\n", port, size);
    return false;
}

bool SuperIO::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    log_spew("SuperIO::IOWrite: port = 0x%x,  size = %u,  value = 0x%x\n", port, size, value);

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

    log_warning("SuperIO::IOWrite: Unhandled write!  port = 0x%x,  size = %u,  value = 0x%x\n", port, size, value);
    return false;
}

}
