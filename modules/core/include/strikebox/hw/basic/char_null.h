#pragma once

#include <cstdint>

#include "char.h"

namespace strikebox {

class NullCharDriver : public CharDriver {
public:
    bool Init() override;
    int Write(const uint8_t *buf, int len) override;
    void AcceptInput() override;
    void Stop() override;

    // IOCTLs
    void SetBreakEnable(bool breakEnable) override;
    void SetSerialParameters(SerialParams *params) override;
};

}
