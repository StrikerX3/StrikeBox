// (C) Ivan "StrikerX3" Oliveira
//
// Datasheet: https://www.onsemi.com/pub/Collateral/ADM1032-D.PDF
#include "strikebox/hw/sm/adm1032.h"

namespace strikebox {

ADM1032Device::~ADM1032Device() {
}

void ADM1032Device::Init() {
}

void ADM1032Device::Reset() {
}

void ADM1032Device::QuickCommand(bool read) {
}

uint8_t ADM1032Device::ReceiveByte() {
    return 0;
}

uint8_t ADM1032Device::ReadByte(uint8_t command) {
    switch (command) {
    case ADM1032RegLocalTemp:
        return 40;
    case ADM1032RegExtTempHigh:
        return 30;
    case ADM1032RegExtTempLow:
        return 0;
    default:
        return 0;
    }
}

uint16_t ADM1032Device::ReadWord(uint8_t command) {
    return ReadByte(command);
}

int ADM1032Device::ReadBlock(uint8_t command, uint8_t *data) {
    return 0;
}

void ADM1032Device::SendByte(uint8_t data) {
}

void ADM1032Device::WriteByte(uint8_t command, uint8_t value) {
}

void ADM1032Device::WriteWord(uint8_t command, uint16_t value) {
}

void ADM1032Device::WriteBlock(uint8_t command, uint8_t* data, int length) {
}

}
