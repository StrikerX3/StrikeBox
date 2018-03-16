#include "serial.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {


#define UART_LCR_DLAB       0x80   /* Divisor latch access bit */

#define UART_IER_MSI        0x08   /* Enable Modem status interrupt */
#define UART_IER_RLSI       0x04   /* Enable receiver line status interrupt */
#define UART_IER_THRI       0x02   /* Enable Transmitter holding register int. */
#define UART_IER_RDI        0x01   /* Enable receiver data interrupt */

#define UART_IIR_NO_INT     0x01   /* No interrupts pending */
#define UART_IIR_ID         0x06   /* Mask for the interrupt ID */

#define UART_IIR_MSI        0x00   /* Modem status interrupt */
#define UART_IIR_THRI       0x02   /* Transmitter holding register empty */
#define UART_IIR_RDI        0x04   /* Receiver data interrupt */
#define UART_IIR_RLSI       0x06   /* Receiver line status interrupt */
#define UART_IIR_CTI        0x0C   /* Character Timeout Indication */

#define UART_IIR_FENF       0x80   /* Fifo enabled, but not functionning */
#define UART_IIR_FE         0xC0   /* Fifo enabled */

/*
 * These are the definitions for the Modem Control Register
 */
#define UART_MCR_LOOP       0x10   /* Enable loopback test mode */
#define UART_MCR_OUT2       0x08   /* Out2 complement */
#define UART_MCR_OUT1       0x04   /* Out1 complement */
#define UART_MCR_RTS        0x02   /* RTS complement */
#define UART_MCR_DTR        0x01   /* DTR complement */

/*
 * These are the definitions for the Modem Status Register
 */
#define UART_MSR_DCD        0x80   /* Data Carrier Detect */
#define UART_MSR_RI         0x40   /* Ring Indicator */
#define UART_MSR_DSR        0x20   /* Data Set Ready */
#define UART_MSR_CTS        0x10   /* Clear to Send */
#define UART_MSR_DDCD       0x08   /* Delta DCD */
#define UART_MSR_TERI       0x04   /* Trailing edge ring indicator */
#define UART_MSR_DDSR       0x02   /* Delta DSR */
#define UART_MSR_DCTS       0x01   /* Delta CTS */
#define UART_MSR_ANY_DELTA  0x0F   /* Any of the delta bits! */

#define UART_LSR_TEMT       0x40   /* Transmitter empty */
#define UART_LSR_THRE       0x20   /* Transmit-hold-register empty */
#define UART_LSR_BI         0x10   /* Break interrupt indicator */
#define UART_LSR_FE         0x08   /* Frame error indicator */
#define UART_LSR_PE         0x04   /* Parity error indicator */
#define UART_LSR_OE         0x02   /* Overrun error indicator */
#define UART_LSR_DR         0x01   /* Receiver data ready */
#define UART_LSR_INT_ANY    0x1E   /* Any of the lsr-interrupt-triggering status bits */

/* Interrupt trigger levels. The byte-counts are for 16550A - in newer UARTs the byte-count for each ITL is higher. */

#define UART_FCR_ITL_1      0x00   /* 1 byte ITL */
#define UART_FCR_ITL_2      0x40   /* 4 bytes ITL */
#define UART_FCR_ITL_3      0x80   /* 8 bytes ITL */
#define UART_FCR_ITL_4      0xC0   /* 14 bytes ITL */

#define UART_FCR_DMS        0x08   /* DMA Mode Select */
#define UART_FCR_XFR        0x04   /* XMIT Fifo Reset */
#define UART_FCR_RFR        0x02   /* RCVR Fifo Reset */
#define UART_FCR_FE         0x01   /* FIFO Enable */

#define MAX_XMIT_RETRY      4

#define SEC_TO_NANO   1000000000ULL

static inline uint64_t GetNanos() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

int Serial::CanReceiveCB(void *userData) {
    return ((Serial *)userData)->CanReceive();
}

void Serial::ReceiveCB(void *userData, const uint8_t *buf, int size) {
    ((Serial *)userData)->Receive(buf, size);
}

void Serial::EventCB(void *userData, int event) {
    ((Serial *)userData)->Event(event);
}

void Serial::UpdateMSLCB(void *userData) {
    ((Serial *)userData)->UpdateMSL();
}

void Serial::FifoTimeoutInterruptCB(void *userData) {
    ((Serial *)userData)->FifoTimeoutInterrupt();
}

Serial::Serial(i8259 *pic, uint32_t ioBase)
    : m_pic(pic)
    , m_ioBase(ioBase)
{
    m_recvFifo = new Fifo<uint8_t>(UART_FIFO_LENGTH);
    m_xmitFifo = new Fifo<uint8_t>(UART_FIFO_LENGTH);

    m_fifoTimeoutTimer = new InvokeLater(UpdateMSLCB, this);
    m_modemStatusPoll = new InvokeLater(FifoTimeoutInterruptCB, this);

    m_baudbase = 115200;
    m_active = false;
    m_recvFifoITL = 14;
    m_lastXmitTs = 0;

    Reset();
}

Serial::~Serial() {
    m_fifoTimeoutTimer->Stop();
    m_modemStatusPoll->Stop();

    delete m_fifoTimeoutTimer;
    delete m_modemStatusPoll;
    delete m_recvFifo;
    delete m_xmitFifo;
}

bool Serial::Init(CharDriver *chr) {
    if (chr == nullptr) {
        log_warning("Serial::Init: No character driver provided\n");
        return false;
    }

    // Set callbacks to the character driver
    m_chr = chr;
    m_chr->m_cbCanReceive = CanReceiveCB;
    m_chr->m_cbReceive = ReceiveCB;
    m_chr->m_cbEvent = EventCB;
    m_chr->m_handler = this;
    
    m_fifoTimeoutTimer->Start();
    m_modemStatusPoll->Start();

    return true;
}

void Serial::Reset() {
    m_rbr = 0;
    m_ier = 0;
    m_iir = UART_IIR_NO_INT;
    m_lcr = 0;
    m_lsr = UART_LSR_TEMT | UART_LSR_THRE;
    m_msr = UART_MSR_DCD | UART_MSR_DSR | UART_MSR_CTS;
    // Default to 9600 baud, 1 start bit, 8 data bits, 1 stop bit, no parity
    m_divider = 0x0C;
    m_mcr = UART_MCR_OUT2;
    m_scr = 0;
    m_tsrRetry = 0;
    m_charTransmitTime = (SEC_TO_NANO / 9600) * 10;
    m_pollMsl = 0;

    m_recvFifo->Clear();
    m_xmitFifo->Clear();

    m_lastXmitTs = GetNanos();

    m_thr_ipending = 0;
    m_lastBreakEnable = 0;
}

void Serial::Stop() {
    m_chr->Stop();
}

bool Serial::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(m_ioBase, PORT_SERIAL_COUNT, this)) return false;
    
    return true;
}

bool Serial::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    port &= 7;
    switch (port) {
    default:
    case 0:
        if (m_lcr & UART_LCR_DLAB) {
            *value = m_divider & 0xff;
        }
        else {
            if (m_fcr & UART_FCR_FE) {
                *value = 0;
                if (!m_recvFifo->IsEmpty()) {
                    m_recvFifo->Pop((uint8_t *)value);
                }
                if (m_recvFifo->Count() == 0) {
                    m_lsr &= ~(UART_LSR_DR | UART_LSR_BI);
                }
                else {
                    m_fifoTimeoutTimer->Set(std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(m_charTransmitTime * 4));
                }
                m_timeoutIpending = 0;
            }
            else {
                *value = m_rbr;
                m_lsr &= ~(UART_LSR_DR | UART_LSR_BI);
#if 0
                if (lastDir != 0) {
                    lastDir = 0;
                    log_debug("\nReceived serial data: ");
                }
                for (int i = 0; i < size; i++) {
                    if (m_rbr >= 20) {
                        log_debug("%c", m_rbr);
                    }
                    else {
                        log_debug(".");
                    }
                }
#endif
            }
            UpdateIRQ();
            if (!(m_mcr & UART_MCR_LOOP)) {
                // In loopback mode, don't receive any data
                m_chr->AcceptInput();
            }
        }
        break;
    case 1:
        if (m_lcr & UART_LCR_DLAB) {
            *value = (m_divider >> 8) & 0xff;
        }
        else {
            *value = m_ier;
        }
        break;
    case 2:
        *value = m_iir;
        if ((*value & UART_IIR_ID) == UART_IIR_THRI) {
            m_thr_ipending = 0;
            UpdateIRQ();
        }
        break;
    case 3:
        *value = m_lcr;
        break;
    case 4:
        *value = m_mcr;
        break;
    case 5:
        *value = m_lsr;
        // Clear break and overrun interrupts
        if (m_lsr & (UART_LSR_BI | UART_LSR_OE)) {
            m_lsr &= ~(UART_LSR_BI | UART_LSR_OE);
            UpdateIRQ();
        }
        break;
    case 6:
        if (m_mcr & UART_MCR_LOOP) {
            // In loopback, the modem output pins are connected to the inputs
            *value = (m_mcr & 0x0c) << 4;
            *value |= (m_mcr & 0x02) << 3;
            *value |= (m_mcr & 0x01) << 5;
        }
        else {
            if (m_pollMsl >= 0) {
                UpdateMSL();
            }
            *value = m_msr;
            // Clear delta bits & msr int after read, if they were set
            if (m_msr & UART_MSR_ANY_DELTA) {
                m_msr &= 0xF0;
                UpdateIRQ();
            }
        }
        break;
    case 7:
        *value = m_scr;
        break;
    }
    
    return true;
}

bool Serial::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    port &= 7;
    switch (port) {
    default:
    case 0:
        if (m_lcr & UART_LCR_DLAB) {
            m_divider = (m_divider & 0xff00) | value;
            UpdateParameters();
        }
        else {
            m_thr = (uint8_t)value;
            if (m_fcr & UART_FCR_FE) {
                // xmit overruns overwrite data, so make space if needed
                if (m_xmitFifo->IsFull()) {
                    m_xmitFifo->Discard();
                }
                m_xmitFifo->Push(m_thr);
                m_lsr &= ~UART_LSR_TEMT;
            }
            m_thr_ipending = 0;
            m_lsr &= ~UART_LSR_THRE;
            UpdateIRQ();
            Transmit();
        }
        break;
    case 1:
        if (m_lcr & UART_LCR_DLAB) {
            m_divider = (m_divider & 0x00ff) | (value << 8);
            UpdateParameters();
        }
        else {
            m_ier = value & 0x0f;
            // If the backend device is a real serial port, turn polling of the modem
            // status lines on physical port on or off depending on UART_IER_MSI state
            if (m_pollMsl >= 0) {
                if (m_ier & UART_IER_MSI) {
                    m_pollMsl = 1;
                    UpdateMSL();
                }
                else {
                    m_modemStatusPoll->Cancel();
                    m_pollMsl = 0;
                }
            }
            if (m_lsr & UART_LSR_THRE) {
                m_thr_ipending = 1;
                UpdateIRQ();
            }
        }
        break;
    case 2:
        value = value & 0xFF;

        if (m_fcr == value) {
            break;
        }

        // Did the enable/disable flag change? If so, make sure FIFOs get flushed
        if ((value ^ m_fcr) & UART_FCR_FE) {
            value |= UART_FCR_XFR | UART_FCR_RFR;
        }

        // FIFO clear
        if (value & UART_FCR_RFR) {
            m_fifoTimeoutTimer->Cancel();
            m_timeoutIpending = 0;
            m_recvFifo->Clear();
        }

        if (value & UART_FCR_XFR) {
            m_xmitFifo->Clear();
        }

        if (value & UART_FCR_FE) {
            m_iir |= UART_IIR_FE;
            // Set recv_fifo trigger level
            switch (value & 0xC0) {
            case UART_FCR_ITL_1:
                m_recvFifoITL = 1;
                break;
            case UART_FCR_ITL_2:
                m_recvFifoITL = 4;
                break;
            case UART_FCR_ITL_3:
                m_recvFifoITL = 8;
                break;
            case UART_FCR_ITL_4:
                m_recvFifoITL = 14;
                break;
            }
        }
        else
            m_iir &= ~UART_IIR_FE;

        // Set fcr - or at least the bits in it that are supposed to "stick"
        m_fcr = value & 0xC9;
        UpdateIRQ();
        break;
    case 3:
    {
        m_lcr = value;
        UpdateParameters();
        bool breakEnable = ((value >> 6) & 1) == 1;
        if (breakEnable != m_lastBreakEnable) {
            m_lastBreakEnable = breakEnable;
            m_chr->SetBreakEnable(breakEnable);
        }
    }
    break;
    case 4:
    {
        int flags = 0;
        int old_mcr = m_mcr;
        m_mcr = value & 0x1f;
        if (value & UART_MCR_LOOP) {
            break;
        }

        if (m_pollMsl >= 0 && old_mcr != m_mcr) {
            // TODO: qemu_chr_fe_ioctl(m_chr, CHR_IOCTL_SERIAL_GET_TIOCM, &flags);

            flags &= ~(CHR_TIOCM_RTS | CHR_TIOCM_DTR);
            
            if (value & UART_MCR_RTS) {
                flags |= CHR_TIOCM_RTS;
            }
            if (value & UART_MCR_DTR) {
                flags |= CHR_TIOCM_DTR;
            }

            // TODO: qemu_chr_fe_ioctl(m_chr, CHR_IOCTL_SERIAL_SET_TIOCM, &flags);

            // Update the modem status after a one-character-send wait-time, since there may be a response
            // from the device/computer at the other end of the serial line
            m_modemStatusPoll->Set(std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(m_charTransmitTime));
        }
    }
    break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        m_scr = value;
        break;
    }

    return true;
}

void Serial::RecvFifoPut(uint8_t chr) {
    // Receive overruns do not overwrite FIFO contents
    if (!m_recvFifo->IsFull()) {
        m_recvFifo->Push(chr);
    }
    else {
        m_lsr |= UART_LSR_OE;
    }
}

void Serial::Transmit() {
    if (m_tsrRetry <= 0) {
        if (m_fcr & UART_FCR_FE) {
            if (m_xmitFifo->IsFull()) {
                m_tsr = 0;
            }
            else {
                m_xmitFifo->Pop(&m_tsr);
            }
            if (m_xmitFifo->IsEmpty()) {
                m_lsr |= UART_LSR_THRE;
            }
        }
        else if ((m_lsr & UART_LSR_THRE)) {
            return;
        }
        else {
            m_tsr = m_thr;
            m_lsr |= UART_LSR_THRE;
            m_lsr &= ~UART_LSR_TEMT;
        }
    }

    if (m_mcr & UART_MCR_LOOP) {
        // in loopback mode, say that we just received a char
        Receive(&m_tsr, 1);
    }
    else if (m_chr->Write(&m_tsr, 1) != 1) {
        if (m_tsrRetry >= 0 && m_tsrRetry < MAX_XMIT_RETRY) {
        // TODO: if (m_tsrRetry >= 0 && m_tsrRetry < MAX_XMIT_RETRY && m_chr->AddWatch(CHR_IO_OUT, SerialTransmit, this) > 0) {
            m_tsrRetry++;
            return;
        }
        m_tsrRetry = 0;
    }
    else {
#if 0
        if (lastDir != 1) {
            lastDir = 1;
            log_debug("\nTransmitted serial data: ");
        }
        if (m_tsr >= 0x20) {
            log_debug("%c", m_tsr);
        }
        else {
            log_debug(".");
        }
#endif
        m_tsrRetry = 0;
    }

    m_lastXmitTs = GetNanos();

    if (m_lsr & UART_LSR_THRE) {
        m_lsr |= UART_LSR_TEMT;
        m_thr_ipending = 1;
        UpdateIRQ();
    }
}

int Serial::CanReceive() {
    if (m_fcr & UART_FCR_FE) {
        if (m_recvFifo->Count() < UART_FIFO_LENGTH) {
            // Advertise (fifo.itl - fifo.count) bytes when count < ITL, and 1
            // if above. If UART_FIFO_LENGTH - fifo.count is advertised the
            // effect will be to almost always fill the fifo completely before
            // the guest has a chance to respond, effectively overriding the
            // ITL that the guest has set.
            return (m_recvFifo->Count() <= m_recvFifoITL)
                ? m_recvFifoITL - m_recvFifo->Count()
                : 1;
        }
        else {
            return 0;
        }
    }
    else {
        return !(m_lsr & UART_LSR_DR);
    }
}

void Serial::Receive(const uint8_t *buf, int size) {
    if (m_fcr & UART_FCR_FE) {
        int i;
        for (i = 0; i < size; i++) {
            RecvFifoPut(buf[i]);
        }
        m_lsr |= UART_LSR_DR;
        // Call the timeout receive callback in 4 char transmit time
        m_fifoTimeoutTimer->Set(std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(m_charTransmitTime * 4));
    }
    else {
        if (m_lsr & UART_LSR_DR) {
            m_lsr |= UART_LSR_OE;
        }
        m_rbr = buf[0];
        m_lsr |= UART_LSR_DR;
    }

    UpdateIRQ();
}

void Serial::ReceiveBreak() {
    m_rbr = 0;
    // When the LSR_DR is set a null byte is pushed into the fifo
    RecvFifoPut('\0');
    m_lsr |= UART_LSR_BI | UART_LSR_DR;
    UpdateIRQ();
}

void Serial::Event(int event) {
    if (event == CHR_EVENT_BREAK) {
        ReceiveBreak();
    }
}

void Serial::FifoTimeoutInterrupt() {
    if (m_recvFifo->Count()) {
        m_timeoutIpending = 1;
        UpdateIRQ();
    }
}

void Serial::UpdateIRQ() {
    uint8_t tmp_iir = UART_IIR_NO_INT;

    if ((m_ier & UART_IER_RLSI) && (m_lsr & UART_LSR_INT_ANY)) {
        tmp_iir = UART_IIR_RLSI;
    }
    else if ((m_ier & UART_IER_RDI) && m_timeoutIpending) {
        // Note that(m_ier & UART_IER_RDI) can mask this interrupt, this is not
        // in the specification but is observed on existing hardware
        tmp_iir = UART_IIR_CTI;
    }
    else if ((m_ier & UART_IER_RDI) && (m_lsr & UART_LSR_DR) && (!(m_fcr & UART_FCR_FE) || m_recvFifo->Count() >= m_recvFifoITL)) {
        tmp_iir = UART_IIR_RDI;
    }
    else if ((m_ier & UART_IER_THRI) && m_thr_ipending) {
        tmp_iir = UART_IIR_THRI;
    }
    else if ((m_ier & UART_IER_MSI) && (m_msr & UART_MSR_ANY_DELTA)) {
        tmp_iir = UART_IIR_MSI;
    }

    m_iir = tmp_iir | (m_iir & 0xF0);

    if (tmp_iir != UART_IIR_NO_INT) {
        m_pic->RaiseIRQ(m_irq);
    }
    else {
        m_pic->LowerIRQ(m_irq);
    }
}

void Serial::UpdateMSL() {
    uint8_t omsr;
    int flags;

    m_modemStatusPoll->Cancel();

    // TODO
    //if (qemu_chr_fe_ioctl(m_chr, CHR_IOCTL_SERIAL_GET_TIOCM, &flags) == -ENOTSUP) {
        m_pollMsl = -1;
        return;
    //}

    // TODO
    /*omsr = m_msr;

    m_msr = (flags & CHR_TIOCM_CTS) ? m_msr | UART_MSR_CTS : m_msr & ~UART_MSR_CTS;
    m_msr = (flags & CHR_TIOCM_DSR) ? m_msr | UART_MSR_DSR : m_msr & ~UART_MSR_DSR;
    m_msr = (flags & CHR_TIOCM_CAR) ? m_msr | UART_MSR_DCD : m_msr & ~UART_MSR_DCD;
    m_msr = (flags & CHR_TIOCM_RI) ? m_msr | UART_MSR_RI : m_msr & ~UART_MSR_RI;

    if (m_msr != omsr) {
        // Set delta bits
        m_msr = m_msr | ((m_msr >> 4) ^ (omsr >> 4));
        // UART_MSR_TERI only if change was from 1 -> 0
        if ((m_msr & UART_MSR_TERI) && !(omsr & UART_MSR_RI)) {
            m_msr &= ~UART_MSR_TERI;
        }
        UpdateIRQ();
    }

    // The real 16550A apparently has a 250ns response latency to line status changes
    // We'll be lazy and poll only every 10ms, and only poll it at all if MSI interrupts are turned on
    if (m_pollMsl) {
        m_modemStatusPoll->Set(std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(SEC_TO_NANO / 100));
    }*/
}

void Serial::UpdateParameters() {
    SerialParams params;

    if (m_divider == 0) {
        return;
    }

    // Start bit
    int frameSize = 1;
    if (m_lcr & 0x08) {
        // Parity bit
        frameSize++;
        if (m_lcr & 0x10) {
            params.parity = 'E';
        }
        else {
            params.parity = 'O';
        }
    }
    else {
        params.parity = 'N';
    }
    if (m_lcr & 0x04) {
        params.stopBits = 2;
    }
    else {
        params.stopBits = 1;
    }

    params.dataBits = (m_lcr & 0x03) + 5;
    params.baudRate = m_baudbase;
    params.divider = m_divider;
    frameSize += params.dataBits + params.stopBits;
    m_charTransmitTime = (GetNanos() / params.baudRate / m_divider) * frameSize;
    m_chr->SetSerialParameters(&params);
}

}
