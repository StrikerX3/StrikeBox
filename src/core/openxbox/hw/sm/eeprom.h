#pragma once

#include "sm.h"

namespace openxbox {

#define EEPROM_SIZE 256

class EEPROMDevice : public SMDevice {
public:
    virtual ~EEPROMDevice();

    // SMDevice functions
    void Init();
    void Reset();

    void QuickCommand(bool read);
    uint8_t ReceiveByte();
    uint8_t ReadByte(uint8_t command);
    uint16_t ReadWord(uint8_t command);
    int ReadBlock(uint8_t command, uint8_t *data);

    void SendByte(uint8_t data);
    void WriteByte(uint8_t command, uint8_t value);
    void WriteWord(uint8_t command, uint16_t value);
    void WriteBlock(uint8_t command, uint8_t* data, int length);

    // EEPROMDevice function
    void SetEEPROM(const uint8_t* pEEPROM) { memcpy(m_pEEPROM, pEEPROM, EEPROM_SIZE); };
private:
    uint8_t m_pEEPROM[EEPROM_SIZE];
};

}
