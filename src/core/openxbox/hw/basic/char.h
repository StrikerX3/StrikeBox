#pragma once

#include <cstdint>

#include "openxbox/cpu.h"
#include "openxbox/util/fifo.h"

namespace openxbox {

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
    int speed;
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
    virtual void Init() = 0;
    virtual int Write(const uint8_t *buf, int len) = 0;
    virtual void AcceptInput() = 0;
    virtual void Poll() = 0;

    // IOCTLs
    virtual void SetBreakEnable(bool breakEnable) = 0;
    virtual void SetSerialParameters(SerialParams *params) = 0;

    // Callbacks
    CanReceiveCallback m_cbCanReceive;
    ReceiveCallback m_cbReceive;
    EventCallback m_cbEvent;
private:
};

}
