#include "char_serial.h"

namespace openxbox {

#define NSENDBUF 2048
#define NRECVBUF 2048
#define MAXCONNECT 1
#define NTIMEOUT 5000

#define READ_BUF_LEN 4096

static int PollCallback(void *data) {
    return ((Win32SerialDriver *)data)->Poll();
}

Win32SerialDriver::Win32SerialDriver(uint8_t portNum, Emulator *emulator)
    : m_portNum(portNum)
    , m_emulator(emulator)
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
    size = sizeof(COMMCONFIG);
    GetDefaultCommConfig(filename, &comcfg, &size);
    comcfg.dcb.DCBlength = sizeof(DCB);
    CommConfigDialog(filename, NULL, &comcfg);

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
    m_emulator->AddPoller(PollCallback, this);

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
        if (m_hsend)
            ret = WriteFile(m_hcom, buf, len, &size, &m_osend);
        else
            ret = WriteFile(m_hcom, buf, len, &size, NULL);
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

int Win32SerialDriver::Poll() {
    COMSTAT status;
    DWORD comerr;

    ClearCommError(m_hcom, &comerr, &status);
    if (status.cbInQue > 0) {
        m_len = status.cbInQue;
        ReadPoll();
        Read();
        return 1;
    }
    return 0;
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
    m_emulator->RemovePoller(PollCallback, this);

    Event(CHR_EVENT_CLOSED);
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
    // TODO: implement
}


}
