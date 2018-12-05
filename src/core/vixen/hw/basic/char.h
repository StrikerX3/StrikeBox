#pragma once

#include <cstdint>

#include "vixen/cpu.h"
#include "vixen/util/fifo.h"

namespace vixen {

#define CHR_EVENT_BREAK   0 // Serial break char
#define CHR_EVENT_FOCUS   1 // Focus to this terminal (modal input needed)
#define CHR_EVENT_OPENED  2 // New connection established
#define CHR_EVENT_MUX_IN  3 // Mux-focus was set to this terminal
#define CHR_EVENT_MUX_OUT 4 // Mux-focus will move on
#define CHR_EVENT_CLOSED  5 // Connection closed

#define CHR_TIOCM_CTS	0x020
#define CHR_TIOCM_CAR	0x040
#define CHR_TIOCM_DSR	0x100
#define CHR_TIOCM_RI	0x080
#define CHR_TIOCM_DTR	0x002
#define CHR_TIOCM_RTS	0x004

#define CHR_IO_IN   0
#define CHR_IO_OUT  1

struct SerialParams {
    int baudRate;
    int divider;
    int parity;
    int dataBits;
    int stopBits;
};

typedef int (*CanReceiveCallback)(void *userData);
typedef void (*ReceiveCallback)(void *userData, const uint8_t *buf, int size);
typedef void (*EventCallback)(void *userData, int event);


/*!
 * Base class for objects that can transmit and receive characters.
 */
class CharDriver {
public:
    virtual bool Init() = 0;
    virtual int Write(const uint8_t *buf, int len) = 0;
    virtual void AcceptInput() = 0;
    virtual void Stop() = 0;

    // IOCTLs
    virtual void SetBreakEnable(bool breakEnable) = 0;
    virtual void SetSerialParameters(SerialParams *params) = 0;

    // Callbacks
    CanReceiveCallback m_cbCanReceive = nullptr;
    ReceiveCallback m_cbReceive = nullptr;
    EventCallback m_cbEvent = nullptr;
    void *m_handler;
protected:
    bool m_open;

    int CanReceive() {
        if (m_cbCanReceive != nullptr && m_handler != nullptr) {
            return m_cbCanReceive(m_handler);
        }
        return 0;
    }

    void Receive(const uint8_t *buf, int size) {
        if (m_cbReceive != nullptr && m_handler != nullptr) {
            m_cbReceive(m_handler, buf, size);
        }
    }

    void Event(int event) {
        // Keep track if the char device is open
        switch (event) {
        case CHR_EVENT_OPENED:
            m_open = true;
            break;
        case CHR_EVENT_CLOSED:
            m_open = false;
            break;
        }

        if (m_cbEvent) {
            m_cbEvent(m_handler, event);
        }
    }
};

}
