#pragma once

#include <cstdint>

#include "char.h"

namespace openxbox {

class NullCharDriver : public CharDriver {
public:
    bool Init() override;
    int Write(const uint8_t *buf, int len) override;
    void AcceptInput() override;
    int Poll() override;

    // IOCTLs
    void SetBreakEnable(bool breakEnable) override;
    void SetSerialParameters(SerialParams *params) override;
};

}
