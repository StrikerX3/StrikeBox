#include "char_serial.h"

namespace strikebox {

#define NSENDBUF 2048
#define NRECVBUF 2048
#define MAXCONNECT 1
#define NTIMEOUT 5000

#define READ_BUF_LEN 4096

void Win32SerialDriver::ReaderFunc(void *userData, uint8_t *buf, uint32_t len) {
    auto driver = ((Win32SerialDriver *)userData);
    while (len > 0) {
        int lenRecv = min(driver->CanReceive(), len);
        if (lenRecv == 0) {
            continue;
        }
        driver->Receive(buf, lenRecv);
        len -= lenRecv;
        buf += lenRecv;
    }
}

void Win32SerialDriver::EventFunc(void *userData, SerialCommEvent evt) {
    ((Win32SerialDriver *)userData)->CommEvent(evt);
}

Win32SerialDriver::Win32SerialDriver(uint8_t portNum) {
    m_comm = new SerialComm(portNum, this, ReaderFunc, EventFunc);
}

Win32SerialDriver::~Win32SerialDriver() {
    m_comm->Stop(true);
    delete m_comm;
}

bool Win32SerialDriver::Init() {
    if (m_comm->Start()) {
        Event(CHR_EVENT_OPENED);
        return true;
    }

    Close();
    return false;
}

int Win32SerialDriver::Write(const uint8_t *buf, int len) {
    m_comm->Write(buf, len);
    return len;
}

void Win32SerialDriver::AcceptInput() {
    // TODO: implement
}

void Win32SerialDriver::Stop() {
    Close();
}

void Win32SerialDriver::Close() {
    m_comm->Stop(true);

    Event(CHR_EVENT_CLOSED);
}

// ----- IOCTLs ---------------------------------------------------------------

void Win32SerialDriver::SetBreakEnable(bool breakEnable) {
    // TODO: implement
}

void Win32SerialDriver::SetSerialParameters(SerialParams *params) {
    log_debug("Win32SerialDriver::SetSerialParameters: Serial port configuration\n");
    log_debug("  Baud rate: %u bps\n", params->baudRate / params->divider);
    log_debug("  Data bits: %u\n", params->dataBits);
    log_debug("  Parity: %c\n", params->parity);
    log_debug("  Stop bits: %u\n", params->stopBits);

    auto commSettings = m_comm->GetSettings();

    commSettings->baudRate = params->baudRate / params->divider;
    commSettings->byteSize = params->dataBits;
    commSettings->stopBits = params->stopBits == 1 ? StopBits_1 : StopBits_2;
    switch (params->parity) {
    case 'E': commSettings->parity = Parity_Even; break;
    case 'O': commSettings->parity = Parity_Odd; break;
    case 'N': commSettings->parity = Parity_None; break;
    }
    
    m_comm->ApplySettings();
}

// ----- Processors -----------------------------------------------------------

void Win32SerialDriver::CommEvent(SerialCommEvent evt) {
#if 0
    switch (evt.type) {
    case SCE_RxChar: log_spew("Win32SerialDriver: Received %u characters\n", evt.Char.numChars); break;
    case SCE_Break:
        log_spew("Win32SerialDriver: Break received\n");
        Event(CHR_EVENT_BREAK);
        break;
    case SCE_Error:
        log_warning("Win32SerialDriver: Receive error(s): ");
        if (evt.ErrorEvent.recvFraming) {
            log_warning(" Framing");
        }
        if (evt.ErrorEvent.recvOverrun) {
            log_warning(" Overrun");
        }
        if (evt.ErrorEvent.recvParity) {
            log_warning(" Parity");
        }
        log_warning("\n");
        break;
    case SCE_TxEmpty: log_spew("Win32SerialDriver: Write buffer emptied\n"); break;
    case SCE_StopRequested: log_debug("Win32SerialDriver: Stop requested\n"); break;
    case SCE_Error_ReadSetupFailed: log_warning("Win32SerialDriver: Read setup failed\n"); break;
    case SCE_Error_ReadProcFailed: log_warning("Win32SerialDriver: Read processing failed\n"); break;
    case SCE_Error_WriteSetupFailed: log_warning("Win32SerialDriver: Write setup failed\n"); break;
    case SCE_Error_WriteProcFailed: log_warning("Win32SerialDriver: Write processing failed\n"); break;
    case SCE_Error_EventWaitFailed: log_warning("Win32SerialDriver: Event setup failed\n"); break;
    case SCE_Error_EventProcFailed: log_warning("Win32SerialDriver: Event processing failed\n"); break;
    case SCE_Error_ConfigFailed: log_warning("Win32SerialDriver: Configuration failed\n"); break;
    case SCE_Error_Unspecified: log_warning("Win32SerialDriver: Unspecified error\n"); break;
    }
#else
    if (evt.type == SCE_Break) {
        Event(CHR_EVENT_BREAK);
    }
#endif
}

}
