#pragma once

#include <cstdint>
#include <windows.h>
#include <mutex>
#include <deque>

namespace openxbox {

enum SerialCommParity {
    Parity_None,
    Parity_Odd,
    Parity_Even,
    Parity_Mark,
    Parity_Space
};

enum SerialCommStopBits {
    StopBits_1,
    StopBits_1_5,
    StopBits_2
};


struct SerialCommSettings {
    uint32_t baudRate;
    uint8_t byteSize;
    SerialCommParity parity;
    SerialCommStopBits stopBits;
    // TODO: many more options...
};


enum SerialCommEventType {
    SCE_RxChar,
    SCE_Break,
    SCE_Error,
    SCE_TxEmpty,

    SCE_StopRequested,

    SCE_Error_ReadSetupFailed,
    SCE_Error_ReadProcFailed,

    SCE_Error_WriteSetupFailed,
    SCE_Error_WriteProcFailed,

    SCE_Error_EventWaitFailed,
    SCE_Error_EventProcFailed,

    SCE_Error_ConfigFailed,

    SCE_Error_Unspecified,
};


struct SerialCommEvent {
    SerialCommEventType type;
    union {
        struct {
            uint32_t numChars;
        } Char;
        struct {
            bool recvFraming;
            bool recvOverrun;
            bool recvParity;
        } ErrorEvent;
    };
};


typedef void(*SerialCommReaderFunc)(void *userData, uint8_t *buf, uint32_t len);
typedef void(*SerialCommEventFunc)(void *userData, SerialCommEvent evt);


class SerialComm {
public:
    SerialComm(uint8_t portNum, void *userData, SerialCommReaderFunc readerFunc, SerialCommEventFunc eventFunc);
    ~SerialComm();

    inline SerialCommSettings *GetSettings() { return &m_settings; }

    void ApplySettings();

    bool Start();
    void Stop(bool waitForStop = false);

    void Write(const uint8_t *buf, uint32_t len);
private:
    void *m_userData;
    uint8_t m_portNum;

    SerialCommSettings m_settings = {
        9600,
        8,
        Parity_None,
        StopBits_1
    };

    struct WriteRequest {
        uint8_t *buf;
        uint32_t len;
    };


    HANDLE m_hComm;

    bool m_running;
    HANDLE m_hStopEvent;

    HANDLE m_hApplySettingsEvent;

    SerialCommReaderFunc m_readerFunc;
    SerialCommEventFunc m_eventFunc;

    HANDLE m_hWriteReqEvent;
    std::mutex m_syncWriteMutex;
    std::mutex m_writeReqMutex;
    std::deque<WriteRequest> m_writeRequests;
    std::mutex m_writeQueueFullMutex;
    std::condition_variable m_writeQueueFullCond;
    std::mutex m_writeQueueEmptyMutex;
    std::condition_variable m_writeQueueEmptyCond;

    std::mutex m_runMutex;

    std::thread m_readerAndEventsThread;
    std::thread m_writerThread;

    static void ReaderAndEventsThreadProc(SerialComm *instance);
    static void WriterThreadProc(SerialComm *instance);

    void ReaderAndEventsLoop();
    void WriterLoop();

    inline void SendRead(uint8_t *buf, uint32_t len) { if (m_readerFunc != nullptr) { m_readerFunc(m_userData, buf, len); } }
    inline void SendEvent(SerialCommEvent evt) { if (m_eventFunc != nullptr) { m_eventFunc(m_userData, evt); } }
};

}
