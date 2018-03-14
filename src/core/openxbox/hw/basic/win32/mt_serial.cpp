#include "mt_serial.h"

#include <cstdio>
#include <thread>

namespace openxbox {

#define WRITE_QUEUE_SIZE 4096

// ----- Helper functions -----------------------------------------------------

static inline BYTE GetWin32Parity(SerialCommParity parity) {
    switch (parity) {
    case Parity_None: return NOPARITY;
    case Parity_Odd: return ODDPARITY;
    case Parity_Even: return EVENPARITY;
    case Parity_Mark: return MARKPARITY;
    case Parity_Space: return SPACEPARITY;
    }
    return 0;
}

static inline BYTE GetWin32StopBits(SerialCommStopBits stopBits) {
    switch (stopBits) {
    case StopBits_1: return ONESTOPBIT;
    case StopBits_1_5: return ONE5STOPBITS;
    case StopBits_2: return TWOSTOPBITS;
    }
    return 0;
}

// ----- SerialComm implementation --------------------------------------------

SerialComm::SerialComm(uint8_t portNum, void *userData, SerialCommReaderFunc readerFunc, SerialCommEventFunc eventFunc)
    : m_portNum(portNum)
    , m_userData(userData)
    , m_readerFunc(readerFunc)
    , m_eventFunc(eventFunc)
{
    m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hWriteReqEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hApplySettingsEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hComm = INVALID_HANDLE_VALUE;

    m_running = false;
}

SerialComm::~SerialComm() {
    Stop(true);
#define CLOSE_HANDLE(h) if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); h = INVALID_HANDLE_VALUE; }
    CLOSE_HANDLE(m_hStopEvent);
    CLOSE_HANDLE(m_hWriteReqEvent);
    CLOSE_HANDLE(m_hApplySettingsEvent);
    CLOSE_HANDLE(m_hComm);
#undef CLOSE_HANDLE
}

bool SerialComm::Start() {
    // Acquire the lock only if we know we're not running
    if (m_running) {
        return true;
    }

    std::lock_guard<std::mutex> lk(m_runMutex);

    // If we happen to acquire the lock and the thread is already running,
    // get out of here immediately
    if (m_running) {
        return true;
    }

    if (m_hStopEvent == INVALID_HANDLE_VALUE) {
        return false;
    }
    if (m_hWriteReqEvent == INVALID_HANDLE_VALUE) {
        return false;
    }

    CHAR portName[11];
    sprintf_s(portName, "\\\\.\\COM%u", m_portNum);

    // Open the serial port
    m_hComm = CreateFileA(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        0);

    if (m_hComm == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Make a local copy of the settings to avoid concurrency issues
    SerialCommSettings settings = m_settings;

    // Configure the serial port
    DCB dcb = { 0 };
    if (!GetCommState(m_hComm, &dcb)) {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
        return false;
    }

    dcb.BaudRate = settings.baudRate;
    dcb.ByteSize = settings.byteSize;
    dcb.fParity = settings.parity != Parity_None;
    dcb.Parity = GetWin32Parity(settings.parity);
    dcb.StopBits = GetWin32StopBits(settings.stopBits);
    dcb.fBinary = TRUE;
    dcb.fInX = FALSE;
    dcb.fOutX = FALSE;

    if (!SetCommState(m_hComm, &dcb)) {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
        return false;
    }

    // Listen to all relevant events
    DWORD dwStoredFlags;

    dwStoredFlags = EV_BREAK | EV_RXCHAR | EV_TXEMPTY | EV_ERR;
    if (!SetCommMask(m_hComm, dwStoredFlags)) {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
        return false;
    }

    // Disable all timeouts; we want to be purely event-driven.
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(m_hComm, &timeouts)) {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
        return false;
    }

    // Mark the thread as running
    m_running = true;

    // Start the threads
    m_readerAndEventsThread = std::thread(ReaderAndEventsThreadProc, this);
    m_writerThread = std::thread(WriterThreadProc, this);

    return true;
}

void SerialComm::Stop(bool waitForStop) {
    if (!m_running) {
        return;
    }

    std::lock_guard<std::mutex> lk(m_runMutex);

    // If we happen to acquire the lock and the thread is no longer running,
    // get out of here immediately
    if (!m_running) {
        return;
    }

    m_running = false;
    SetEvent(m_hStopEvent);

    // Send a poison pill to the writer thread, pushing it to the front
    // and ignoring queue size restrictions
    {
        std::lock_guard<std::mutex> lk(m_writeReqMutex);

        // Enqueue the poison pill, denoted by a null buffer
        m_writeRequests.push_front(WriteRequest{ nullptr, 0 });

        // Signal the write request event
        SetEvent(m_hWriteReqEvent);

        // Notify that the queue is no longer empty
        {
            std::unique_lock<std::mutex> lkQueueFull(m_writeQueueEmptyMutex);
            m_writeQueueEmptyCond.notify_one();
        }
    }

    if (waitForStop) {
        if (m_readerAndEventsThread.joinable()) m_readerAndEventsThread.join();
        if (m_writerThread.joinable()) m_writerThread.join();
    }
}

void SerialComm::ApplySettings() {
    std::lock_guard<std::mutex> lk(m_settingsMutex);

    SetEvent(m_hApplySettingsEvent);
}

void SerialComm::Write(const uint8_t *buf, uint32_t len) {
    // Wait until the write queue is not full
    while (m_writeRequests.size() >= WRITE_QUEUE_SIZE) {
        std::unique_lock<std::mutex> lkQueueFull(m_writeQueueFullMutex);
        m_writeQueueFullCond.wait(lkQueueFull, [&]() -> bool { return m_writeRequests.size() < WRITE_QUEUE_SIZE; });
    }

    std::lock_guard<std::mutex> lk(m_writeReqMutex);

    // Make a copy of the write request buffer
    uint8_t *bufCopy = new uint8_t[len];
    memcpy_s(bufCopy, len, buf, len);

    // Enqueue the write request
    m_writeRequests.push_back(WriteRequest{ bufCopy, len });

    // Signal the write request event
    SetEvent(m_hWriteReqEvent);

    // Notify that the queue is no longer empty
    {
        std::unique_lock<std::mutex> lkQueueFull(m_writeQueueEmptyMutex);
        m_writeQueueEmptyCond.notify_one();
    }
}

void SerialComm::ReaderAndEventsThreadProc(SerialComm *instance) {
    instance->ReaderAndEventsLoop();
}

void SerialComm::WriterThreadProc(SerialComm *instance) {
    instance->WriterLoop();
}

void SerialComm::ReaderAndEventsLoop() {
    // Make a local copy of the settings to avoid concurrency issues
    SerialCommSettings settings = m_settings;

    OVERLAPPED ovlRead = { 0 };
    OVERLAPPED ovlEvent = { 0 };

    ovlRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    ovlEvent.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    HANDLE hEvents[] = {
        ovlRead.hEvent,
        ovlEvent.hEvent,
        m_hApplySettingsEvent,
        m_hStopEvent
    };

    // Read buffer
    uint8_t *readBuf = nullptr;

    // Determines if we have an outstanding read or eventoperation
    bool outstandingRead = false;
    bool outstandingEvent = false;

    DWORD dwRead;
    DWORD dwCommEvent;

    DWORD readRequestLen = 0;

    while (m_running) {
        // If we don't have an outstanding read request and there are bytes to read, issue a read
        if (!outstandingRead && readRequestLen > 0) {
            readBuf = new uint8_t[readRequestLen];
            if (!ReadFile(m_hComm, readBuf, readRequestLen, &dwRead, &ovlRead)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    // Read will complete in the future
                    outstandingRead = true;
                }
                else {
                    // Failed to read
                    SendEvent(SerialCommEvent{ SCE_Error_ReadSetupFailed });
                }
            }
            else {
                // FIXME: code repetition could be avoided

                // Read completed immediately
                outstandingRead = false;

                // Send to callback
                SendRead(readBuf, dwRead);

                // Clean up
                delete[] readBuf;
                readBuf = nullptr;
                readRequestLen = 0;
                ResetEvent(ovlRead.hEvent);
            }
        }

        // If we don't have any outstanding events, ask for one
        if (!outstandingEvent) {
            if (!WaitCommEvent(m_hComm, &dwCommEvent, &ovlEvent)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    outstandingEvent = true;
                }
                else {
                    // Failed to wait for an event
                    SendEvent(SerialCommEvent{ SCE_Error_EventWaitFailed });
                }
            }
            else {
                // FIXME: code repetition could be avoided

                // WaitCommEvent returned immediately
                outstandingEvent = false;

                // Received characters
                if (dwCommEvent & EV_RXCHAR) {
                    // Get how many bytes can be read to issue a read
                    COMSTAT comStat;
                    DWORD dwErrors;
                    if (!ClearCommError(m_hComm, &dwErrors, &comStat)) {
                        SendEvent(SerialCommEvent{ SCE_Error_EventProcFailed });
                    }
                    else {
                        readRequestLen = comStat.cbInQue;
                    }

                    // Build event data
                    SerialCommEvent evt;
                    evt.type = SCE_RxChar;
                    evt.Char.numChars = readRequestLen;

                    // Send to callback
                    SendEvent(evt);
                }

                // Break event
                if (dwCommEvent & EV_BREAK) {
                    // Send to callback
                    SendEvent(SerialCommEvent{ SCE_Break });
                }

                // Error event
                if (dwCommEvent & EV_ERR) {
                    // Get the errors
                    COMSTAT comStat;
                    DWORD dwErrors;
                    if (!ClearCommError(m_hComm, &dwErrors, &comStat)) {
                        SendEvent(SerialCommEvent{ SCE_Error_EventProcFailed });
                    }
                    else {
                        // Build event data
                        SerialCommEvent evt;
                        evt.type = SCE_Error;
                        evt.ErrorEvent.recvFraming = (dwErrors & CE_FRAME) != 0;
                        evt.ErrorEvent.recvOverrun = (dwErrors & CE_OVERRUN) != 0;
                        evt.ErrorEvent.recvParity = (dwErrors & CE_RXPARITY) != 0;

                        // Send to callback
                        SendEvent(evt);
                    }
                }

                // Write buffer empty
                if (dwCommEvent & EV_TXEMPTY) {
                    // Send to callback
                    SendEvent(SerialCommEvent{ SCE_TxEmpty });
                }

                // Clean up
                ResetEvent(ovlEvent.hEvent);
            }
        }

        // Don't bother waiting if we have no outstanding operations
        if (!outstandingRead && !outstandingEvent) {
            continue;
        }

        // Wait for an event
        DWORD waitResult = WaitForMultipleObjects(sizeof(hEvents) / sizeof(hEvents[0]), hEvents, FALSE, INFINITE);

        switch (waitResult) {
        case WAIT_OBJECT_0:  // Reader overlay
        {
            // Outstanding read operation completed
            if (!GetOverlappedResult(m_hComm, &ovlRead, &dwRead, FALSE)) {
                // Send error event on read
                SendEvent(SerialCommEvent{ SCE_Error_ReadProcFailed });
            }
            else {
                // FIXME: code repetition could be avoided

                // Read completed successfully
                outstandingRead = false;

                // Send to callback
                SendRead(readBuf, dwRead);

                // Clean up
                delete[] readBuf;
                readBuf = nullptr;
                readRequestLen = 0;
                ResetEvent(ovlRead.hEvent);
            }
            break;
        }
        case WAIT_OBJECT_0 + 1:  // Event overlay
        {
            DWORD result;
            if (!GetOverlappedResult(m_hComm, &ovlEvent, &result, FALSE)) {
                SendEvent(SerialCommEvent{ SCE_Error_EventProcFailed });
            }
            else {
                // FIXME: code repetition could be avoided

                // WaitCommEvent finished
                outstandingEvent = false;

                // Received characters
                if (dwCommEvent & EV_RXCHAR) {
                    // Get how many bytes can be read to issue a read
                    COMSTAT comStat;
                    DWORD dwErrors;
                    if (!ClearCommError(m_hComm, &dwErrors, &comStat)) {
                        SendEvent(SerialCommEvent{ SCE_Error_EventProcFailed });
                    }
                    else {
                        readRequestLen = comStat.cbInQue;
                    }

                    // Build event data
                    SerialCommEvent evt;
                    evt.type = SCE_RxChar;
                    evt.Char.numChars = readRequestLen;

                    // Send to callback
                    SendEvent(evt);
                }

                // Break event
                if (dwCommEvent & EV_BREAK) {
                    // Send to callback
                    SendEvent(SerialCommEvent{ SCE_Break });
                }

                // Error event
                if (dwCommEvent & EV_ERR) {
                    // Get the errors
                    COMSTAT comStat;
                    DWORD dwErrors;
                    if (!ClearCommError(m_hComm, &dwErrors, &comStat)) {
                        SendEvent(SerialCommEvent{ SCE_Error_EventProcFailed });
                    }
                    else {
                        // Build event data
                        SerialCommEvent evt;
                        evt.type = SCE_Error;
                        evt.ErrorEvent.recvFraming = (dwErrors & CE_FRAME) != 0;
                        evt.ErrorEvent.recvOverrun = (dwErrors & CE_OVERRUN) != 0;
                        evt.ErrorEvent.recvParity = (dwErrors & CE_RXPARITY) != 0;

                        // Send to callback
                        SendEvent(evt);
                    }
                }

                // Write buffer empty
                if (dwCommEvent & EV_TXEMPTY) {
                    // Send to callback
                    SendEvent(SerialCommEvent{ SCE_TxEmpty });
                }

                // Clean up
                ResetEvent(ovlEvent.hEvent);
            }
            break;
        }
        case WAIT_OBJECT_0 + 2:  // Apply settings
        {
            std::lock_guard<std::mutex> lk(m_settingsMutex);

            // Make a copy of the new settings
            SerialCommSettings newSettings = m_settings;

            // Apply changes as needed
            if ((newSettings.baudRate != settings.baudRate) ||
                (newSettings.byteSize != settings.byteSize) ||
                (newSettings.parity != settings.parity) ||
                (newSettings.stopBits != settings.stopBits)) {

                // Reconfigure serial port
                DCB dcb = { 0 };
                if (!GetCommState(m_hComm, &dcb)) {
                    SendEvent(SerialCommEvent{ SCE_Error_ConfigFailed });
                }
                else {
                    dcb.BaudRate = newSettings.baudRate;
                    dcb.ByteSize = newSettings.byteSize;
                    dcb.fParity = newSettings.parity != Parity_None;
                    dcb.Parity = GetWin32Parity(newSettings.parity);
                    dcb.StopBits = GetWin32StopBits(newSettings.stopBits);

                    if (!SetCommState(m_hComm, &dcb)) {
                        SendEvent(SerialCommEvent{ SCE_Error_ConfigFailed });
                    }
                }
            }

            // Copy new settings to current settings
            settings = newSettings;
            ResetEvent(m_hApplySettingsEvent);
            break;
        }
        case WAIT_OBJECT_0 + 3:  // Stop request
        {
            // Send stop request event
            SendEvent(SerialCommEvent{ SCE_StopRequested });

            // The loop will exit immediately after this
            break;
        }
        default:
        {
            // Probably an error
            SendEvent(SerialCommEvent{ SCE_Error_Unspecified });
            break;
        }
        }
    }

    if (readBuf != nullptr) delete[] readBuf;
}

void SerialComm::WriterLoop() {
    // Make a local copy of the settings to avoid concurrency issues
    SerialCommSettings settings = m_settings;

    // Create the writer overlapped structure
    OVERLAPPED ovlWrite = { 0 };
    ovlWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Setup wait events
    HANDLE hEvents[] = {
        ovlWrite.hEvent,
        m_hWriteReqEvent,
        m_hStopEvent
    };

    // Write buffer
    uint8_t *writeBuf = nullptr;

    // Determines if we have an outstanding write operation
    bool outstandingWrite = false;

    // The number of bytes written
    DWORD dwWrite;

    while (m_running) {
        // If there are enqueued write requests and we don't have an outstanding write request,
        // pull one item from the queue and begin the write request
        if (!outstandingWrite) {
            // Wait until an item is made available

            // Try to pop one item from the queue
            WriteRequest req;
            bool available = false;
            {
                // Wait until the write queue is not empty
                while (m_writeRequests.size() == 0) {
                    std::unique_lock<std::mutex> lkQueueEmpty(m_writeQueueEmptyMutex);
                    m_writeQueueEmptyCond.wait(lkQueueEmpty, [&]() -> bool { return m_writeRequests.size() > 0; });
                }

                std::lock_guard<std::mutex> lk(m_writeReqMutex);
                if (m_writeRequests.size() > 0) {
                    req = m_writeRequests.front();
                    m_writeRequests.pop_front();

                    // We got the poison pill; die
                    if (req.buf == nullptr) {
                        break;
                    }

                    available = true;

                    // Notify that the queue is no longer full
                    std::unique_lock<std::mutex> lkQueueFull(m_writeQueueFullMutex);
                    m_writeQueueFullCond.notify_one();
                }
            }

            // If we could pop an item, issue the write
            if (available) {
                if (!WriteFile(m_hComm, req.buf, req.len, &dwWrite, &ovlWrite)) {
                    if (GetLastError() != ERROR_IO_PENDING) {
                        // Error in communications; report it.
                        SendEvent(SerialCommEvent{ SCE_Error_WriteSetupFailed });
                    }
                    else {
                        outstandingWrite = true;
                        writeBuf = req.buf;
                    }
                }
                else {
                    // FIXME: code repetition could be avoided

                    // Write completed immediately
                    outstandingWrite = false;

                    // Clean up
                    delete[] req.buf;
                    ResetEvent(ovlWrite.hEvent);
                }
            }
        }

        // Don't bother waiting if we have no outstanding operations
        if (!outstandingWrite) {
            continue;
        }

        // Wait for an event
        DWORD waitResult = WaitForMultipleObjects(sizeof(hEvents) / sizeof(hEvents[0]), hEvents, FALSE, INFINITE);

        switch (waitResult) {
        case WAIT_OBJECT_0:  // Write overlay
        {
            // Outstanding write operation completed
            if (!GetOverlappedResult(m_hComm, &ovlWrite, &dwWrite, FALSE)) {
                SendEvent(SerialCommEvent{ SCE_Error_WriteProcFailed });
            }
            else {
                // FIXME: code repetition could be avoided

                // Write operation completed successfully
                outstandingWrite = false;

                // Clean up
                delete[] writeBuf;
                writeBuf = nullptr;
                ResetEvent(ovlWrite.hEvent);
            }
            break;
        }
        case WAIT_OBJECT_0 + 1:  // Write request
        {
            // Loop back to the beginning where the pending write request will be pulled and initiated
            ResetEvent(m_hWriteReqEvent);
            break;
        }
        case WAIT_OBJECT_0 + 2:  // Stop request
        {
            // Nothing to do here, really. The loop will exit immediately after this
            break;
        }
        default:
        {
            // Probably an error
            SendEvent(SerialCommEvent{ SCE_Error_Unspecified });
            break;
        }
        }
    }

    if (writeBuf != nullptr) delete[] writeBuf;
    for (auto it = m_writeRequests.begin(); it != m_writeRequests.end(); it++) {
        if (it->buf != nullptr) delete[] it->buf;
    }
}

}
