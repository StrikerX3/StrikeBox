#include "eeprom.h"

#include <memory>

namespace vixen {

EEPROMDevice::~EEPROMDevice() {
}

void EEPROMDevice::Init() {
    // TODO
}

void EEPROMDevice::Reset() {
    // TODO
}

void EEPROMDevice::QuickCommand(bool read) {
    // TODO
}

uint8_t EEPROMDevice::ReceiveByte() {
    return 0; // TODO
}

uint8_t EEPROMDevice::ReadByte(uint8_t command) {
    return *(m_pEEPROM + command);
}

uint16_t EEPROMDevice::ReadWord(uint8_t command) {
    return *((uint16_t*)(m_pEEPROM + command));
}

int EEPROMDevice::ReadBlock(uint8_t command, uint8_t *data) {
    return 0; // TODO
}

void EEPROMDevice::SendByte(uint8_t data) {
    // TODO
}

void EEPROMDevice::WriteByte(uint8_t command, uint8_t value) {
    *((uint8_t*)(m_pEEPROM + command)) = value;
}

void EEPROMDevice::WriteWord(uint8_t command, uint16_t value) {
    *((uint16_t*)(m_pEEPROM + command)) = value;
}

void EEPROMDevice::WriteBlock(uint8_t command, uint8_t* data, int length) {
    memcpy(m_pEEPROM + command, data, length);
}

}
