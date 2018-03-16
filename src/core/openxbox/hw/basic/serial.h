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
#define PORT_SERIAL_BASE_2   0x2F8

#define PORT_SERIAL_COUNT    7


class Serial : public IODevice {
public:
    Serial(i8259 *pic, uint32_t ioBase);
    ~Serial();

    bool Init(CharDriver *chr);
    void Reset();
    void Stop();
    
    inline void SetIRQ(uint8_t irq) { m_irq = irq; }
    inline void SetBaudBase(int baudBase) { m_baudbase = baudBase; }

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
    uint32_t m_ioBase;

    uint16_t m_divider = 0;
    uint8_t m_rbr = 0; // receive register
    uint8_t m_thr = 0; // transmit holding register
    uint8_t m_tsr = 0; // transmit shift register
    uint8_t m_ier = 0;
    uint8_t m_iir = 0; // read only
    uint8_t m_lcr = 0;
    uint8_t m_mcr = 0;
    uint8_t m_lsr = 0; // read only
    uint8_t m_msr = 0; // read only
    uint8_t m_scr = 0;
    uint8_t m_fcr = 0;
    
    // NOTE: this hidden state is necessary for tx irq generation as it can be reset while reading iir
    int m_thr_ipending = 0;
    CharDriver *m_chr;
    bool m_lastBreakEnable = false;
    int m_itShift = 0;
    int m_baudbase = 0;
    int m_tsrRetry = 0;

    uint8_t m_irq = 0;

    // Time when the last byte was successfully sent out of the tsr
    uint64_t m_lastXmitTs = 0;
    Fifo<uint8_t> *m_recvFifo;
    Fifo<uint8_t> *m_xmitFifo;
    // Interrupt trigger level for recv_fifo
    uint8_t m_recvFifoITL;

    InvokeLater *m_fifoTimeoutTimer;
    int m_timeoutIpending = 0;  // timeout interrupt pending state

    uint64_t m_charTransmitTime = 0; // time to transmit a char in ticks
    int m_pollMsl = 0;

    InvokeLater *m_modemStatusPoll;

    int lastDir = -1;
};

}
