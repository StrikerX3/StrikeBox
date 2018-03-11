#pragma once

#include <cstdint>

#include "openxbox/cpu.h"
#include "openxbox/util/fifo.h"
#include "openxbox/util/invoke_later.h"
#include "char.h"
#include "../basic/i8259.h"

#include <chrono>

namespace openxbox {

#define UART_FIFO_LENGTH 16

#define PORT_SERIAL_BASE_1   0x3F8
#define PORT_SERIAL_END_1    0x3FF
#define PORT_SERIAL_COUNT_1  (PORT_SERIAL_END_1 - PORT_SERIAL_BASE_1 + 1)

#define PORT_SERIAL_BASE_2   0x2F8
#define PORT_SERIAL_END_2    0x2FF
#define PORT_SERIAL_COUNT_2  (PORT_SERIAL_END_2 - PORT_SERIAL_BASE_2 + 1)

class Serial : public IODevice {
public:
    Serial(i8259 *pic);
    ~Serial();

    bool Init(CharDriver *chr);
    void Reset();
    
    inline void SetIRQ(uint8_t irq) { m_irq = irq; }

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size);
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size);

    bool m_active;

private:
    int CanReceive();
    void Receive(const uint8_t *buf, int size);
    void ReceiveBreak();
    
    void Event(int event);

    void RecvFifoPut(uint8_t chr);
    void Transmit();

    void FifoTimeoutInterrupt();
    
    void UpdateIRQ();
    void UpdateMSL();
    void UpdateParameters();

    static int CanReceiveCB(void *userData);
    static void ReceiveCB(void *userData, const uint8_t *buf, int size);
    static void EventCB(void *userData, int event);
    static void UpdateMSLCB(void *userData);
    static void FifoTimeoutInterruptCB(void *userData);

    i8259 *m_pic;

    uint16_t m_divider;
    uint8_t m_rbr; // receive register
    uint8_t m_thr; // transmit holding register
    uint8_t m_tsr; // transmit shift register
    uint8_t m_ier;
    uint8_t m_iir; // read only
    uint8_t m_lcr;
    uint8_t m_mcr;
    uint8_t m_lsr; // read only
    uint8_t m_msr; // read only
    uint8_t m_scr;
    uint8_t m_fcr;
    
    // NOTE: this hidden state is necessary for tx irq generation as it can be reset while reading iir
    int m_thr_ipending;
    CharDriver *m_chr;
    bool m_lastBreakEnable;
    int m_itShift;
    int m_baudbase;
    int m_tsrRetry;

    uint8_t m_irq;

    // Time when the last byte was successfully sent out of the tsr
    uint64_t m_lastXmitTs;
    Fifo<uint8_t> *m_recvFifo;
    Fifo<uint8_t> *m_xmitFifo;
    // Interrupt trigger level for recv_fifo
    uint8_t m_recvFifoItl;

    InvokeLater *m_fifoTimeoutTimer;
    int m_timeoutIpending;  // timeout interrupt pending state

    uint64_t m_charTransmitTime; // time to transmit a char in ticks
    int m_pollMsl;

    InvokeLater *m_modemStatusPoll;
};

}
