#include "char_null.h"

namespace openxbox {

bool NullCharDriver::Init() {
    // Nothing to initialize
    return true;
}

int NullCharDriver::Write(const uint8_t *buf, int len) {
    // Discard everything

#ifdef _DEBUG
    for (int i = 0; i < len; i++) {
        if (buf[i] >= 0x20) {
            log_debug("%c", buf[i]);
        }
        else {
            log_debug(".");
        }
    }
#endif
    return 1;
}

void NullCharDriver::AcceptInput() {
    // Nothing to do
}

int NullCharDriver::Poll() {
    // Nothing to do
    return 0;
}

// ----- IOCTLs ---------------------------------------------------------------

void NullCharDriver::SetBreakEnable(bool breakEnable) {
    // Nothing to do
}

void NullCharDriver::SetSerialParameters(SerialParams *params) {
    // Nothing to do
}

}
