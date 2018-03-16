#include "char_null.h"

namespace openxbox {

bool NullCharDriver::Init() {
    // Nothing to initialize
    return true;
}

int NullCharDriver::Write(const uint8_t *buf, int len) {
    // Discard everything
    return 1;
}

void NullCharDriver::AcceptInput() {
    // Nothing to do
}

void NullCharDriver::Stop() {
    // Nothing to do
}

// ----- IOCTLs ---------------------------------------------------------------

void NullCharDriver::SetBreakEnable(bool breakEnable) {
    // Nothing to do
}

void NullCharDriver::SetSerialParameters(SerialParams *params) {
    // Nothing to do
}

}
