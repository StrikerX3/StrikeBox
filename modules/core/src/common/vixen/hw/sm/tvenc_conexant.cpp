#include "vixen/hw/sm/tvenc_conexant.h"

namespace vixen {

// This is just a completely fake device that doesn't respond to anything.
// Its mere presence is enough to satisfy the X-codes initialization process.

TVEncConexantDevice::~TVEncConexantDevice() {
}

void TVEncConexantDevice::Init() {
    // TODO
}

void TVEncConexantDevice::Reset() {
    // TODO
}

void TVEncConexantDevice::QuickCommand(bool read) {
    // TODO
}

uint8_t TVEncConexantDevice::ReceiveByte() {
    return 0; // TODO
}

uint8_t TVEncConexantDevice::ReadByte(uint8_t command) {
    return *(m_registers + command);
}

uint16_t TVEncConexantDevice::ReadWord(uint8_t command) {
    return *((uint16_t*)(m_registers + command));
}

int TVEncConexantDevice::ReadBlock(uint8_t command, uint8_t *data) {
    return 0; // TODO
}

void TVEncConexantDevice::SendByte(uint8_t data) {
    // TODO
}

void TVEncConexantDevice::WriteByte(uint8_t command, uint8_t value) {
    *((uint8_t*)(m_registers + command)) = value;
}

void TVEncConexantDevice::WriteWord(uint8_t command, uint16_t value) {
    *((uint16_t*)(m_registers + command)) = value;
}

void TVEncConexantDevice::WriteBlock(uint8_t command, uint8_t* data, int length) {
    memcpy(m_registers + command, data, length);
}

}
