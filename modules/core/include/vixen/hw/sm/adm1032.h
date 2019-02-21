// ADM1032 emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Datasheet: https://www.onsemi.com/pub/Collateral/ADM1032-D.PDF
#pragma once

#include "sm.h"

namespace vixen {

enum ADM1032Register {
    ADM1032RegLocalTemp = 0x00,    // (Read-only) Local Temperature Value
    ADM1032RegExtTempHigh = 0x01,  // (Read-only) External Temperature Value high byte
    ADM1032RegExtTempLow = 0x10,   // (Read-only) External Temperature Value low byte  (only 3 most significant bits used, 0.125 C increments)
};

class ADM1032Device : public SMDevice {
public:
    virtual ~ADM1032Device();

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
};

}
