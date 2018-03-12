#include "char_serial.h"

namespace openxbox {

#define NSENDBUF 2048
#define NRECVBUF 2048
#define MAXCONNECT 1
#define NTIMEOUT 5000

#define READ_BUF_LEN 4096

static uint32_t PollCallback(void *data) {
    return ((Win32SerialDriver *)data)->Poll();
}

Win32SerialDriver::Win32SerialDriver(uint8_t portNum)
    : m_portNum(portNum)
{
}

bool Win32SerialDriver::Init() {
    COMMCONFIG comcfg;
    COMMTIMEOUTS cto = { 0, 0, 0, 0, 0 };
    COMSTAT comstat;
    DWORD size;
    DWORD err;

    m_hsend = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hsend) {
        log_warning("Win32SerialDriver::Init: CreateEvent (send) failed: %lu\n", GetLastError());
        goto fail;
    }
    m_hrecv = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hrecv) {
        log_warning("Win32SerialDriver::Init: CreateEvent (recv) failed: %lu\n", GetLastError());
        goto fail;
    }

    char filename[MAX_PATH];
    sprintf(filename, "\\\\.\\COM%d", m_portNum);

    m_hcom = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if (m_hcom == INVALID_HANDLE_VALUE) {
        log_warning("Win32SerialDriver::Init: CreateFile failed: %lu\n", GetLastError());
        m_hcom = NULL;
        goto fail;
    }

    if (!SetupComm(m_hcom, NRECVBUF, NSENDBUF)) {
        log_warning("Win32SerialDriver::Init: SetupComm failed: %lu\n", GetLastError());
        goto fail;
    }

    ZeroMemory(&comcfg, sizeof(COMMCONFIG));
    comcfg.dcb.DCBlength = sizeof(DCB);
    size = sizeof(COMMCONFIG);
    GetDefaultCommConfig(filename, &comcfg, &size);

    DWORD szConfig;
    if (!GetCommConfig(m_hcom, &comcfg, &szConfig)) {
        log_warning("Win32SerialDriver::Init: GetCommConfig failed: %lu\n", GetLastError());
        goto fail;
    }

    comcfg.dcb.fBinary = TRUE;
    if (!SetCommState(m_hcom, &comcfg.dcb)) {
        log_warning("Win32SerialDriver::Init: SetCommState failed: %lu\n", GetLastError());
        goto fail;
    }

    if (!SetCommMask(m_hcom, EV_ERR)) {
        log_warning("Failed SetCommMask\n");
        goto fail;
    }

    cto.ReadIntervalTimeout = MAXDWORD;
    if (!SetCommTimeouts(m_hcom, &cto)) {
        log_warning("Win32SerialDriver::Init: SetCommTimeouts failed: %lu\n", GetLastError());
        goto fail;
    }

    if (!ClearCommError(m_hcom, &err, &comstat)) {
        log_warning("Win32SerialDriver::Init: ClearCommError failed: %lu\n", GetLastError());
        goto fail;
    }
    m_runPoller = true;
    m_pollerThread = Thread_Create("[HW] Serial port polling thread", PollCallback, this);

    Event(CHR_EVENT_OPENED);
    return true;

fail:
    Close();
    return false;
}

int Win32SerialDriver::Write(const uint8_t *buf, int len1) {
    DWORD len, ret, size, err;

    len = len1;
    ZeroMemory(&m_osend, sizeof(m_osend));
    m_osend.hEvent = m_hsend;
    while (len > 0) {
        if (m_hsend) {
            ret = WriteFile(m_hcom, buf, len, &size, &m_osend);
        }
        else {
            ret = WriteFile(m_hcom, buf, len, &size, NULL);
        }
        if (!ret) {
            err = GetLastError();
            if (err == ERROR_IO_PENDING) {
                ret = GetOverlappedResult(m_hcom, &m_osend, &size, TRUE);
                if (ret) {
                    buf += size;
                    len -= size;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
        else {
            buf += size;
            len -= size;
        }
    }
    return len1 - len;
}

void Win32SerialDriver::AcceptInput() {
    // TODO: implement
}

void Win32SerialDriver::Close() {
    if (m_hsend) {
        CloseHandle(m_hsend);
        m_hsend = NULL;
    }
    if (m_hrecv) {
        CloseHandle(m_hrecv);
        m_hrecv = NULL;
    }
    if (m_hcom) {
        CloseHandle(m_hcom);
        m_hcom = NULL;
    }
    m_runPoller = false;

    Event(CHR_EVENT_CLOSED);
}

int Win32SerialDriver::Poll() {
    while (m_runPoller) {
        COMSTAT status;
        DWORD comerr;

        ClearCommError(m_hcom, &comerr, &status);
        if (status.cbInQue > 0) {
            m_len = status.cbInQue;
            ReadPoll();
            Read();
        }
        /*ReadPoll();
        m_len = m_maxSize;
        Read();*/
    }

    return 0;
}

int Win32SerialDriver::ReadPoll() {
    m_maxSize = CanReceive();
    return m_maxSize;
}

void Win32SerialDriver::Read() {
    if (m_len > m_maxSize) {
        m_len = m_maxSize;
    }
    if (m_len == 0) {
        return;
    }

    DoReadFile();
}

void Win32SerialDriver::DoReadFile() {
    int ret, err;
    uint8_t buf[READ_BUF_LEN];
    DWORD size;
    DWORD dwEventMask;

    /*if (!SetCommMask(m_hcom, EV_RXCHAR)) {
        log_warning("Win32SerialDriver::DoReadFile: SetCommMask failed: %ul\n", GetLastError());
        return;
    }

    if (!WaitCommEvent(m_hcom, &dwEventMask, NULL)) {
        log_warning("Win32SerialDriver::DoReadFile: WaitCommEvent failed: %ul\n", GetLastError());
        return;
    }*/

    ZeroMemory(&m_orecv, sizeof(m_orecv));
    m_orecv.hEvent = m_hrecv;
    ret = ReadFile(m_hcom, buf, m_len, &size, &m_orecv);
    if (!ret) {
        err = GetLastError();
        if (err == ERROR_IO_PENDING) {
            ret = GetOverlappedResult(m_hcom, &m_orecv, &size, TRUE);
        }
    }

    if (size > 0) {
        Receive(buf, size);
    }
}

// ----- IOCTLs ---------------------------------------------------------------

void Win32SerialDriver::SetBreakEnable(bool breakEnable) {
    // TODO: implement
}

void Win32SerialDriver::SetSerialParameters(SerialParams *params) {
    DCB config;
    config.DCBlength = sizeof(DCB);
    if (!GetCommState(m_hcom, &config)) {
        log_warning("Win32SerialDriver::SetSerialParameters: GetCommState failed: %lu\n", GetLastError());
        return;
    }
    
    config.BaudRate = params->baudRate / params->divider;
    config.ByteSize = params->dataBits;
    config.StopBits = params->stopBits == 1 ? ONESTOPBIT : TWOSTOPBITS;
    switch (params->parity) {
    case 'E': config.Parity = EVENPARITY; break;
    case 'O': config.Parity = ODDPARITY; break;
    case 'N': config.Parity = NOPARITY; break;
    }
    
    if (!SetCommState(m_hcom, &config)) {
        log_warning("Win32SerialDriver::SetSerialParameters: SetCommState failed: %lu\n", GetLastError());
    }
    log_debug("Win32SerialDriver::SetSerialParameters: Serial port configuration\n");
    log_debug("  Baud rate: %u bps\n", config.BaudRate);
    log_debug("  Byte size: %u bits\n", config.ByteSize);
    log_debug("  Stop bits: %u\n", config.StopBits);
    log_debug("  Parity: %c\n", params->parity);
}


}
