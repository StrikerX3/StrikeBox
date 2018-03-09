#include "tvenc_conexant.h"

namespace openxbox {

// This is just a completely fake device that doesn't respond to anything.
// Its mere presence is enough to satisfy the X-codes initialization process.

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
    return 0; // TODO
}

uint16_t TVEncConexantDevice::ReadWord(uint8_t command) {
    return 0; // TODO
}

int TVEncConexantDevice::ReadBlock(uint8_t command, uint8_t *data) {
    return 0; // TODO
}

void TVEncConexantDevice::SendByte(uint8_t data) {
    // TODO
}

void TVEncConexantDevice::WriteByte(uint8_t command, uint8_t value) {
    // TODO
}

void TVEncConexantDevice::WriteWord(uint8_t command, uint16_t value) {
    // TODO
}

void TVEncConexantDevice::WriteBlock(uint8_t command, uint8_t* data, int length) {
    // TOOD
}

}
