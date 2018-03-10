#pragma once

#include "sm.h"

namespace openxbox {

class EEPROMDevice : public SMDevice {
public:
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
    void SetEEPROM(uint8_t* pEEPROM) { m_pEEPROM = pEEPROM; };
private:
    uint8_t *m_pEEPROM;
};

}
