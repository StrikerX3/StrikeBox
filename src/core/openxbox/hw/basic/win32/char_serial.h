#pragma once

#include "../char.h"
#include "openxbox/emulator.h"

#include <windows.h>

namespace openxbox {

class Win32SerialDriver : public CharDriver {
public:
    Win32SerialDriver(uint8_t portNum, Emulator *emulator);

    bool Init() override;
    int Write(const uint8_t *buf, int len) override;
    void AcceptInput() override;
    int Poll() override;

    // IOCTLs
    void SetBreakEnable(bool breakEnable) override;
    void SetSerialParameters(SerialParams *params) override;

private:
    void Close();

    int ReadPoll();
    void Read();
    void DoReadFile();

    Emulator *m_emulator;

    uint8_t m_portNum;

    int m_maxSize;
    HANDLE m_hcom, m_hrecv, m_hsend;
    OVERLAPPED m_orecv, m_osend;
    BOOL m_fpipe;
    DWORD m_len;

    friend int PollCallback(void *data);
};

}
