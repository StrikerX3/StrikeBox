#pragma once

#include <cstdint>

#include "char.h"

namespace openxbox {

class NullCharDriver : public CharDriver {
public:
    void Init();
    int Write(const uint8_t *buf, int len);
    void AcceptInput();
    void Poll();

    // IOCTLs
    void SetBreakEnable(bool breakEnable);
    void SetSerialParameters(SerialParams *params);
};

}
