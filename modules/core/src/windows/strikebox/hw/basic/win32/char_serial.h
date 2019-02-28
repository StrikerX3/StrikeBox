#pragma once

#include "strikebox/hw/basic/char.h"
#include "strikebox/thread.h"
#include "mt_serial.h"

#include <windows.h>

namespace strikebox {

class Win32SerialDriver : public CharDriver {
public:
    Win32SerialDriver(uint8_t portNum);
    virtual ~Win32SerialDriver();

    bool Init() override;
    int Write(const uint8_t *buf, int len) override;
    void AcceptInput() override;
    void Stop() override;

    // IOCTLs
    void SetBreakEnable(bool breakEnable) override;
    void SetSerialParameters(SerialParams *params) override;

private:
    SerialComm *m_comm;

    void Close();

    static void ReaderFunc(void *userData, uint8_t *buf, uint32_t len);
    static void EventFunc(void *userData, SerialCommEvent evt);

    void CommEvent(SerialCommEvent evt);
};

}
