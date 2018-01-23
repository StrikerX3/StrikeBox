/*
 * Copyright (C) 2017  Matt Borgerson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gdbserver.h"

#ifdef _WIN32
	int inet_aton(const char *cp, struct in_addr *inp) { return InetPton(AF_INET, cp, inp); }
	int close(SOCKET socket) { return closesocket(socket); }

#   define printNetErrorMessage do { \
		int errCode = WSAGetLastError(); \
		LPSTR errString = NULL; \
		int size = FormatMessage( \
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, \
			0, \
			errCode, \
			0, \
			(LPSTR)&errString, \
			0, \
			0); \
		fprintf(stderr, "Error: %s\n", errString); \
		LocalFree(errString); \
	} while (0);

#   define initNetwork do { \
		WSADATA wsaData; \
		int err = WSAStartup(MAKEWORD(2, 2), &wsaData); \
		if (err != 0) { \
			printNetErrorMessage; \
			return 1; \
		} \
	} while (0);

#   define shutdownNetwork do { \
		int err = WSACleanup(); \
		if (err != 0) { \
			printNetErrorMessage; \
		} \
	} while (0);

#else
#   define printNetErrorMessage fprintf(stderr, "Error: %s\n", neterrstr)
#   define initNetwork
#   define shutdownNetwork
#endif

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif
#define ASSERT assert

/*!
 * Constructor
 */
GdbServer::GdbServer(Cpu *cpu, const char *bind_host, int bind_port)
: m_cpu(cpu), m_bind_host(bind_host), m_bind_port(bind_port)
{
}

/*!
 * Destructor
 */
GdbServer::~GdbServer()
{
}

/*!
 * Initialize
 */
int GdbServer::Initialize()
{
    int status;
    int val;

	initNetwork;

    /* Fill local address struct */
    m_bind_addr.sin_family = AF_INET;
    m_bind_addr.sin_port = htons(m_bind_port);
    if (!inet_aton(m_bind_host, &(m_bind_addr.sin_addr))) {
        fprintf(stderr, "Error: Invalid bind host address format\n");
        return 1;
    }

    /* Create the socket */
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        printNetErrorMessage;
        return 1;
    }

    /* Allow re-using a local address */
    val = 1;
    status = setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
    if (status) {
        printNetErrorMessage;
        close(m_sockfd);
        return 1;
    }

    /* No delays */
    val = 1;
    status = setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));
    if (status) {
        printNetErrorMessage;
        close(m_sockfd);
        return 1;
    }

    /* Bind the socket to an address/port */
    status = bind(m_sockfd, (struct sockaddr *)&m_bind_addr, sizeof(m_bind_addr));
    if (status) {
        printNetErrorMessage;
        close(m_sockfd);
        return 1;
    }

    /* Begin waiting for connections */
    status = listen(m_sockfd, 10);
    if (status) {
        printNetErrorMessage;
        close(m_sockfd);
        return 1;
    }
    
    return 0;
}

/*!
 * Close active connections
 */
int GdbServer::Shutdown()
{
    printf("Stopping server\n");
    if (m_peer_sockfd >= 0) {
        close(m_peer_sockfd);
    }
    if (m_sockfd >= 0) {
        close(m_sockfd);
    }
	shutdownNetwork;
    return 0;
}

/*!
 * Start listening for connections
 */
int GdbServer::WaitForConnection()
{
    int status;
    int val;

    /* Wait for an incoming connection */
    fprintf(stderr, "Waiting for new connection at %s:%d\n",
                    inet_ntoa(m_bind_addr.sin_addr),
                    ntohs(m_bind_addr.sin_port));
    m_peer_addr_len = sizeof(m_peer_addr);
    status = accept(m_sockfd, (struct sockaddr *)&m_peer_addr, &m_peer_addr_len);
    if (status < 0) {
		printNetErrorMessage;
        return 1;
    }
    
    /* New connection received! */
    assert(m_peer_addr_len >= sizeof(m_peer_addr));
    assert(m_peer_addr.sin_family == AF_INET);
    fprintf(stderr, "New connection from %s:%d\n",
                    inet_ntoa(m_peer_addr.sin_addr),
                    ntohs(m_peer_addr.sin_port));
    m_peer_sockfd = status;

    val = 1;
    status = setsockopt(m_peer_sockfd, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));
    if (status) {
        printNetErrorMessage;
        close(m_peer_sockfd);
        close(m_sockfd);
        return 1;
    }

    return 0;
}

/*!
 * Main handler for a client, called when connection is made
 */
int GdbServer::Debug(int signal)
{
    int result;

    if (m_peer_sockfd < 0) {
        // No client!
        return 1;
    }

    memset(&m_dbg_state.registers, 0, sizeof(m_dbg_state.registers));

    m_dbg_state.signum = signal;

    // Load Registers
    m_cpu->RegRead(REG_EAX,     &m_dbg_state.registers[DBG_CPU_I386_REG_EAX]);
    m_cpu->RegRead(REG_ECX,     &m_dbg_state.registers[DBG_CPU_I386_REG_ECX]);
    m_cpu->RegRead(REG_EDX,     &m_dbg_state.registers[DBG_CPU_I386_REG_EDX]);
    m_cpu->RegRead(REG_EBX,     &m_dbg_state.registers[DBG_CPU_I386_REG_EBX]);
    m_cpu->RegRead(REG_ESP,     &m_dbg_state.registers[DBG_CPU_I386_REG_ESP]);
    m_cpu->RegRead(REG_EBP,     &m_dbg_state.registers[DBG_CPU_I386_REG_EBP]);
    m_cpu->RegRead(REG_ESI,     &m_dbg_state.registers[DBG_CPU_I386_REG_ESI]);
    m_cpu->RegRead(REG_EDI,     &m_dbg_state.registers[DBG_CPU_I386_REG_EDI]);
    m_cpu->RegRead(REG_EIP,     &m_dbg_state.registers[DBG_CPU_I386_REG_PC]);
    m_cpu->RegRead(REG_CS,      &m_dbg_state.registers[DBG_CPU_I386_REG_CS]);
    m_cpu->RegRead(REG_EFLAGS,  &m_dbg_state.registers[DBG_CPU_I386_REG_PS]);
    m_cpu->RegRead(REG_SS,      &m_dbg_state.registers[DBG_CPU_I386_REG_SS]);
    m_cpu->RegRead(REG_DS,      &m_dbg_state.registers[DBG_CPU_I386_REG_DS]);
    m_cpu->RegRead(REG_ES,      &m_dbg_state.registers[DBG_CPU_I386_REG_ES]);
    m_cpu->RegRead(REG_FS,      &m_dbg_state.registers[DBG_CPU_I386_REG_FS]);
    m_cpu->RegRead(REG_GS,      &m_dbg_state.registers[DBG_CPU_I386_REG_GS]);

    // If interrupt was caused by a soft breakpoint (int3 = CCh), EIP will point
    // to the instruction *after* the int3 instruction, which is probably in
    // the middle of the instruction that we are breaking on. In this case, roll
    // EIP back by 1. GDB client will replace the soft breakpoint with correct
    // instruction byte before continuing execution.
    if (signal == 3) {
        m_dbg_state.registers[DBG_CPU_I386_REG_PC] -= 1;
    }

    // Begin debugging episode
    result = dbg_main();
    if (result != 0) {
        return result;
    }

#if 1
    // Restore registers
    m_cpu->RegWrite(REG_EAX,     m_dbg_state.registers[DBG_CPU_I386_REG_EAX]);
    m_cpu->RegWrite(REG_ECX,     m_dbg_state.registers[DBG_CPU_I386_REG_ECX]);
    m_cpu->RegWrite(REG_EDX,     m_dbg_state.registers[DBG_CPU_I386_REG_EDX]);
    m_cpu->RegWrite(REG_EBX,     m_dbg_state.registers[DBG_CPU_I386_REG_EBX]);
    m_cpu->RegWrite(REG_ESP,     m_dbg_state.registers[DBG_CPU_I386_REG_ESP]);
    m_cpu->RegWrite(REG_EBP,     m_dbg_state.registers[DBG_CPU_I386_REG_EBP]);
    m_cpu->RegWrite(REG_ESI,     m_dbg_state.registers[DBG_CPU_I386_REG_ESI]);
    m_cpu->RegWrite(REG_EDI,     m_dbg_state.registers[DBG_CPU_I386_REG_EDI]);
    m_cpu->RegWrite(REG_EIP,     m_dbg_state.registers[DBG_CPU_I386_REG_PC]);
    // m_cpu->RegWrite(REG_CS,      m_dbg_state.registers[DBG_CPU_I386_REG_CS]);
    m_cpu->RegWrite(REG_EFLAGS,  m_dbg_state.registers[DBG_CPU_I386_REG_PS]);
    // m_cpu->RegWrite(REG_SS,      m_dbg_state.registers[DBG_CPU_I386_REG_SS]);
    // m_cpu->RegWrite(REG_DS,      m_dbg_state.registers[DBG_CPU_I386_REG_DS]);
    // m_cpu->RegWrite(REG_ES,      m_dbg_state.registers[DBG_CPU_I386_REG_ES]);
    // m_cpu->RegWrite(REG_FS,      m_dbg_state.registers[DBG_CPU_I386_REG_FS]);
    // m_cpu->RegWrite(REG_GS,      m_dbg_state.registers[DBG_CPU_I386_REG_GS]);
#endif

    return 0;
}

/*!
 * Receive a byte from the client
 */
int GdbServer::dbg_sys_getc(void)
{
    char buf;
    ssize_t bytes_received;

    if (m_peer_sockfd < 0) {
        return EOF;
    }
    
    bytes_received = recv(m_peer_sockfd, &buf, sizeof(buf), 0);

    if (bytes_received > 0) {
        /* Success */
        return buf;
    } else if (bytes_received == 0) {
        /* Peer has shutdown */
        fprintf(stderr, "Client disconnected\n");
        return EOF;
    } else {
        /* Error! */
        fprintf(stderr, "Error receiving data from client!\n");
        return EOF;
    }
}

/*!
 * Send a byte to the client
 */
int GdbServer::dbg_sys_putchar(int ch)
{
    char buf = (char)ch;
    ssize_t bytes_sent;

    if (m_peer_sockfd < 0) {
        return EOF;
    }
    
    bytes_sent = send(m_peer_sockfd, &buf, sizeof(buf), 0);

    if (bytes_sent != sizeof(buf)) {
        fprintf(stderr, "Error sending data to client!\n");
        return EOF;
    }
    
    return 0;
}

/*!
 * Read a byte from system memory
 */
int GdbServer::dbg_sys_mem_readb(address addr, char *val)
{
    m_cpu->MemRead(addr, 1, val);
    return 0;
}

/*!
 * Write a byte to system memory
 */
int GdbServer::dbg_sys_mem_writeb(address addr, char val)
{
    m_cpu->MemWrite(addr, 1, &val);
    return 0;
}

/*!
 * Allow CPU to continue execution
 */
int GdbServer::dbg_sys_continue()
{
    m_dbg_state.registers[DBG_CPU_I386_REG_PS] &= ~(1<<8);
    return 0;
}

/*!
 * Single step CPU execution
 */
int GdbServer::dbg_sys_step()
{
	m_dbg_state.registers[DBG_CPU_I386_REG_PS] |= 1<<8;
    return 0;
}

/*
 * Get integer value for a string representation.
 *
 * If the string starts with + or -, it will be signed accordingly.
 *
 * If base == 0, the base will be determined:
 *   base 16 if the string starts with 0x or 0X,
 *   base 10 otherwise
 *
 * If endptr is specified, it will point to the last non-digit in the
 * string. If there are no digits in the string, it will be set to NULL.
 */
int GdbServer::dbg_strtol(const char *str, size_t len, int base, const char **endptr)
{
    size_t pos;
    int sign, tmp, value, valid;

    value = 0;
    pos   = 0;
    sign  = 1;
    valid = 0;

    if (endptr) {
        *endptr = NULL;
    }

    if (len < 1) {
        return 0;
    }

    /* Detect negative numbers */
    if (str[pos] == '-') {
        sign = -1;
        pos += 1;
    } else if (str[pos] == '+') {
        sign = 1;
        pos += 1;
    }

    /* Detect '0x' hex prefix */
    if ((pos + 2 < len) && (str[pos] == '0') &&
        ((str[pos+1] == 'x') || (str[pos+1] == 'X'))) {
        base = 16;
        pos += 2;
    }

    if (base == 0) {
        base = 10;
    }

    for (; (pos < len) && (str[pos] != '\x00'); pos++) {
        tmp = dbg_get_val(str[pos], base);
        if (tmp == EOF) {
            break;
        }

        value = value*base + tmp;
        valid = 1; /* At least one digit is valid */
    }

    if (!valid) {
        return 0;
    }

    if (endptr) {
        *endptr = str+pos;
    }

    value *= sign;

    return value;
}

/*
 * Get the corresponding ASCII hex digit character for a value.
 */
char GdbServer::dbg_get_digit(int val)
{
    const char digits[] = "0123456789abcdef";

    if ((val >= 0) && (val <= 0xf)) {
        return digits[val];
    } else {
        return EOF;
    }
}

/*
 * Get the corresponding value for a ASCII digit character.
 *
 * Supports bases 2-16.
 */
int GdbServer::dbg_get_val(char digit, int base)
{
    int value;

    if ((digit >= '0') && (digit <= '9')) {
        value = digit-'0';
    } else if ((digit >= 'a') && (digit <= 'f')) {
        value = digit-'a'+0xa;
    } else if ((digit >= 'A') && (digit <= 'F')) {
        value = digit-'A'+0xa;
    } else {
        return EOF;
    }

    return (value < base) ? value : EOF;
}

/*
 * Determine if this is a printable ASCII character.
 */
int GdbServer::dbg_is_printable_char(char ch)
{
    return (ch >= 0x20 && ch <= 0x7e);
}

/*!
 * Receive a packet acknowledgment
 *
 * Returns:
 *    0   if an ACK (+) was received
 *    1   if a NACK (-) was received
 *    EOF otherwise
 */
int GdbServer::dbg_recv_ack(void)
{
    int response;

    /* Wait for packet ack */
    switch (response = dbg_sys_getc()) {
    case '+':
        /* Packet acknowledged */
        return 0;
    case '-':
        /* Packet negative acknowledged */
        return 1;
    case EOF:
        /* Failed to recieve data from client */
        return EOF;
    default:
        /* Bad response! */
        DEBUG_PRINT("received bad packet response: 0x%2x\n", response);
        return EOF;
    }
}

/*!
 * Calculate 8-bit checksum of a buffer.
 *
 * Returns:
 *    8-bit checksum.
 */
int GdbServer::dbg_checksum(const char *buf, size_t len)
{
    unsigned char csum;

    csum = 0;

    while (len--) {
        csum += *buf++;
    }

    return csum;
}

/*!
 * Transmits a packet of data.
 * Packets are of the form: $<packet-data>#<checksum>
 *
 * Returns:
 *    0   if the packet was transmitted and acknowledged
 *    1   if the packet was transmitted but not acknowledged
 *    EOF otherwise
 */
int GdbServer::dbg_send_packet(const char *pkt_data, size_t pkt_len)
{
    char buf[3];
    char csum;

    /* Send packet start */
    if (dbg_sys_putchar('$') == EOF) {
        return EOF;
    }

#if DEBUG
    {
        size_t p;
        DEBUG_PRINT("-> ");
        for (p = 0; p < pkt_len; p++) {
            if (dbg_is_printable_char(pkt_data[p])) {
                DEBUG_PRINT("%c", pkt_data[p]);
            } else {
                DEBUG_PRINT("\\x%02x", pkt_data[p]&0xff);
            }
        }
        DEBUG_PRINT("\n");
    }
#endif

    /* Send packet data */
    if (dbg_write(pkt_data, pkt_len) == EOF) {
        return EOF;
    }

    /* Send the checksum */
    buf[0] = '#';
    csum = dbg_checksum(pkt_data, pkt_len);
    if ((dbg_enc_hex(buf+1, sizeof(buf)-1, &csum, 1) == EOF) ||
        (dbg_write(buf, sizeof(buf)) == EOF)) {
        return EOF;
    }

    return dbg_recv_ack();
}

/*!
 * Receives a packet of data, assuming a 7-bit clean connection.
 *
 * Returns:
 *    0   if the packet was received
 *    EOF otherwise
 */
int GdbServer::dbg_recv_packet(char *pkt_buf, size_t pkt_buf_len, size_t *pkt_len)
{
    int data;
    char expected_csum, actual_csum;
    char buf[2];

    /* Wait for packet start */
    actual_csum = 0;

    while (1) {
        data = dbg_sys_getc();
        if (data == '$') {
            /* Detected start of packet. */
            break;
        } else if (data == EOF) {
            return EOF;
        }
    }

    /* Read until checksum */
    *pkt_len = 0;
    while (1) {
        data = dbg_sys_getc();

        if (data == EOF) {
            /* Error receiving character */
            return EOF;
        } else if (data == '#') {
            /* End of packet */
            break;
        } else {
            /* Check for space */
            if (*pkt_len >= pkt_buf_len) {
                DEBUG_PRINT("packet buffer overflow\n");
                return EOF;
            }

            /* Store character and update checksum */
            pkt_buf[(*pkt_len)++] = (char) data;
        }
    }

#if DEBUG
    {
        size_t p;
        DEBUG_PRINT("<- ");
        for (p = 0; p < *pkt_len; p++) {
            if (dbg_is_printable_char(pkt_buf[p])) {
                DEBUG_PRINT("%c", pkt_buf[p]);
            } else {
                DEBUG_PRINT("\\x%02x", pkt_buf[p] & 0xff);
            }
        }
        DEBUG_PRINT("\n");
    }
#endif

    /* Receive the checksum */
    if ((dbg_read(buf, sizeof(buf), 2) == EOF) ||
        (dbg_dec_hex(buf, 2, &expected_csum, 1) == EOF)) {
        return EOF;
    }

    /* Verify checksum */
    actual_csum = dbg_checksum(pkt_buf, *pkt_len);
    if (actual_csum != expected_csum) {
        /* Send packet nack */
        DEBUG_PRINT("received packet with bad checksum\n");
        dbg_sys_putchar('-');
        return EOF;
    }

    /* Send packet ack */
    dbg_sys_putchar('+');
    return 0;
}

/*****************************************************************************
 * Data Encoding/Decoding
 ****************************************************************************/

/*!
 * Encode data to its hex-value representation in a buffer.
 *
 * Returns:
 *    0+  number of bytes written to buf
 *    EOF if the buffer is too small
 */
int GdbServer::dbg_enc_hex(char *buf, size_t buf_len, const char *data, size_t data_len)
{
    size_t pos;

    if (buf_len < data_len*2) {
        /* Buffer too small */
        return EOF;
    }

    for (pos = 0; pos < data_len; pos++) {
        *buf++ = dbg_get_digit((data[pos] >> 4) & 0xf);
        *buf++ = dbg_get_digit((data[pos]     ) & 0xf);
    }

    return data_len*2;
}

/*!
 * Decode data from its hex-value representation to a buffer.
 *
 * Returns:
 *    0   if successful
 *    EOF if the buffer is too small
 */
int GdbServer::dbg_dec_hex(const char *buf, size_t buf_len, char *data, size_t data_len)
{
    size_t pos;
    int tmp;

    if (buf_len != data_len*2) {
        /* Buffer too small */
        return EOF;
    }

    for (pos = 0; pos < data_len; pos++) {
        /* Decode high nibble */
        tmp = dbg_get_val(*buf++, 16);
        if (tmp == EOF) {
            /* Buffer contained junk. */
            ASSERT(0);
            return EOF;
        }

        data[pos] = tmp << 4;

        /* Decode low nibble */
        tmp = dbg_get_val(*buf++, 16);
        if (tmp == EOF) {
            /* Buffer contained junk. */
            ASSERT(0);
            return EOF;
        }
        data[pos] |= tmp;
    }

    return 0;
}

/*!
 * Encode data to its binary representation in a buffer.
 *
 * Returns:
 *    0+  number of bytes written to buf
 *    EOF if the buffer is too small
 */
int GdbServer::dbg_enc_bin(char *buf, size_t buf_len, const char *data, size_t data_len)
{
    size_t buf_pos, data_pos;

    for (buf_pos = 0, data_pos = 0; data_pos < data_len; data_pos++) {
        if (data[data_pos] == '$' ||
            data[data_pos] == '#' ||
            data[data_pos] == '}' ||
            data[data_pos] == '*') {
            if (buf_pos+1 >= buf_len) {
                ASSERT(0);
                return EOF;
            }
            buf[buf_pos++] = '}';
            buf[buf_pos++] = data[data_pos] ^ 0x20;
        } else {
            if (buf_pos >= buf_len) {
                ASSERT(0);
                return EOF;
            }
            buf[buf_pos++] = data[data_pos];
        }
    }

    return buf_pos;
}

/*!
 * Decode data from its bin-value representation to a buffer.
 *
 * Returns:
 *    0+  if successful, number of bytes decoded
 *    EOF if the buffer is too small
 */
int GdbServer::dbg_dec_bin(const char *buf, size_t buf_len, char *data, size_t data_len)
{
    size_t buf_pos, data_pos;

    for (buf_pos = 0, data_pos = 0; buf_pos < buf_len; buf_pos++) {
        if (data_pos >= data_len) {
            /* Output buffer overflow */
            ASSERT(0);
            return EOF;
        }
        if (buf[buf_pos] == '}') {
            /* The next byte is escaped! */
            if (buf_pos+1 >= buf_len) {
                /* There's an escape character, but no escaped character
                 * following the escape character. */
                ASSERT(0);
                return EOF;
            }
            buf_pos += 1;
            data[data_pos++] = buf[buf_pos] ^ 0x20;
        } else {
            data[data_pos++] = buf[buf_pos];
        }
    }

    return data_pos;
}

/*!
 * Read from memory and encode into buf.
 *
 * Returns:
 *    0+  number of bytes written to buf
 *    EOF if the buffer is too small
 */
int GdbServer::dbg_mem_read(char *buf, size_t buf_len, address addr, size_t len, dbg_enc_func enc)
{
    char data[64];
    size_t pos;

    if (len > sizeof(data)) {
        return EOF;
    }

    /* Read from system memory */
    for (pos = 0; pos < len; pos++) {
        if (dbg_sys_mem_readb(addr+pos, &data[pos])) {
            /* Failed to read */
            return EOF;
        }
    }

    /* Encode data */
    return enc(buf, buf_len, data, len);
}

/*!
 * Write to memory from encoded buf.
 */
int GdbServer::dbg_mem_write(const char *buf, size_t buf_len, address addr, size_t len, dbg_dec_func dec)
{
    char data[64];
    size_t pos;

    if (len > sizeof(data)) {
        return EOF;
    }

    /* Decode data */
    if (dec(buf, buf_len, data, len) == EOF) {
        return EOF;
    }

    /* Write to system memory */
    for (pos = 0; pos < len; pos++) {
        if (dbg_sys_mem_writeb(addr+pos, data[pos])) {
            /* Failed to write */
            return EOF;
        }
    }

    return 0;
}

/*!
 * Send OK packet
 */
int GdbServer::dbg_send_ok_packet(char *buf, size_t buf_len)
{
    return dbg_send_packet("OK", 2);
}

/*!
 * Send a message to the debugging console (via O XX... packet)
 */
int GdbServer::dbg_send_conmsg_packet(char *buf, size_t buf_len, const char *msg)
{
    size_t size;
    int status;

    if (buf_len < 2) {
        /* Buffer too small */
        return EOF;
    }

    buf[0] = 'O';
    status = dbg_enc_hex(&buf[1], buf_len-1, msg, strlen(msg));
    if (status == EOF) {
        return EOF;
    }
    size = 1 + status;
    return dbg_send_packet(buf, size);
}

/*!
 * Send a signal packet (S AA).
 */
int GdbServer::dbg_send_signal_packet(char *buf, size_t buf_len, char signal)
{
    size_t size;
    int status;

    if (buf_len < 4) {
        /* Buffer too small */
        return EOF;
    }

    buf[0] = 'S';
    status = dbg_enc_hex(&buf[1], buf_len-1, &signal, 1);
    if (status == EOF) {
        return EOF;
    }
    size = 1 + status;
    return dbg_send_packet(buf, size);
}

/*!
 * Send an error packet (E AA).
 */
int GdbServer::dbg_send_error_packet(char *buf, size_t buf_len, char error)
{
    size_t size;
    int status;

    if (buf_len < 4) {
        /* Buffer too small */
        return EOF;
    }

    buf[0] = 'E';
    status = dbg_enc_hex(&buf[1], buf_len-1, &error, 1);
    if (status == EOF) {
        return EOF;
    }
    size = 1 + status;
    return dbg_send_packet(buf, size);
}

/*!
 * Write a sequence of bytes.
 *
 * Returns:
 *    0   if successful
 *    EOF if failed to write all bytes
 */
int GdbServer::dbg_write(const char *buf, size_t len)
{
    while (len--) {
        if (dbg_sys_putchar(*buf++) == EOF) {
            return EOF;
        }
    }

    return 0;
}

/*!
 * Read a sequence of bytes.
 *
 * Returns:
 *    0   if successfully read len bytes
 *    EOF if failed to read all bytes
 */
int GdbServer::dbg_read(char *buf, size_t buf_len, size_t len)
{
    char c;

    if (buf_len < len) {
        /* Buffer too small */
        return EOF;
    }

    while (len--) {
        if ((c = dbg_sys_getc()) == EOF) {
            return EOF;
        }
        *buf++ = c;
    }

    return 0;
}

/*
 * Main debug loop. Handles commands.
 */
int GdbServer::dbg_main()
{
    address     addr;
    char        pkt_buf[256];
    int         status;
    size_t      length;
    size_t      pkt_len;
    const char *ptr_next;

    if (dbg_send_signal_packet(pkt_buf, sizeof(pkt_buf), 0) == EOF) {
        return EOF;
    }

    while (1) {
        /* Receive the next packet */
        status = dbg_recv_packet(pkt_buf, sizeof(pkt_buf), &pkt_len);
        if (status == EOF) {
            break;
        }

        if (pkt_len == 0) {
            /* Received empty packet.. */
            continue;
        }

        ptr_next = pkt_buf;

        /*
         * Handle one letter commands
         */
        switch (pkt_buf[0]) {

        /* Calculate remaining space in packet from ptr_next position. */
        #define token_remaining_buf (pkt_len-(ptr_next-pkt_buf))

        /* Expecting a seperator. If not present, go to error */
        #define token_expect_seperator(c) \
            { \
                if (!ptr_next || *ptr_next != c) { \
                    goto error; \
                } else { \
                    ptr_next += 1; \
                } \
            }

        /* Expecting an integer argument. If not present, go to error */
        #define token_expect_integer_arg(arg) \
            { \
                arg = dbg_strtol(ptr_next, token_remaining_buf, \
                                 16, &ptr_next); \
                if (!ptr_next) { \
                    goto error; \
                } \
            }

        /*
         * Read Registers
         * Command Format: g
         */
        case 'g':
            /* Encode registers */
            status = dbg_enc_hex(pkt_buf, sizeof(pkt_buf),
                                 (char *)&(m_dbg_state.registers),
                                 sizeof(m_dbg_state.registers));
            if (status == EOF) {
                goto error;
            }
            pkt_len = status;
            status = dbg_send_packet(pkt_buf, pkt_len);
            break;
        
        /*
         * Write Registers
         * Command Format: G XX...
         */
        case 'G':
            status = dbg_dec_hex(pkt_buf+1, pkt_len-1,
                                 (char *)&(m_dbg_state.registers),
                                 sizeof(m_dbg_state.registers));
            if (status == EOF) {
                goto error;
            }
            status = dbg_send_ok_packet(pkt_buf, sizeof(pkt_buf));
            break;

        /*
         * Read a Register
         * Command Format: p n
         */
        case 'p':
            ptr_next += 1;
            token_expect_integer_arg(addr);

            if (addr >= DBG_CPU_I386_NUM_REGISTERS) {
                goto error;
            }

            /* Read Register */
            status = dbg_enc_hex(pkt_buf, sizeof(pkt_buf),
                                 (char *)&(m_dbg_state.registers[addr]),
                                 sizeof(m_dbg_state.registers[addr]));
            if (status == EOF) {
                goto error;
            }
            status = dbg_send_packet(pkt_buf, status);
            break;
        
        /*
         * Write a Register
         * Command Format: P n...=r...
         */
        case 'P':
            ptr_next += 1;
            token_expect_integer_arg(addr);
            token_expect_seperator('=');

            if (addr >= DBG_CPU_I386_NUM_REGISTERS) {
                goto error;
            }

            status = dbg_dec_hex(ptr_next, token_remaining_buf,
                                 (char *)&(m_dbg_state.registers[addr]),
                                 sizeof(m_dbg_state.registers[addr]));
            if (status == EOF) {
                goto error;
            }
            status = dbg_send_ok_packet(pkt_buf, sizeof(pkt_buf));
            break;
        
        /*
         * Read Memory
         * Command Format: m addr,length
         */
        case 'm':
            ptr_next += 1;
            token_expect_integer_arg(addr);
            token_expect_seperator(',');
            token_expect_integer_arg(length);

            /* Read Memory */
            status = dbg_mem_read(pkt_buf, sizeof(pkt_buf),
                                  addr, length, dbg_enc_hex);
            if (status == EOF) {
                goto error;
            }
            status = dbg_send_packet(pkt_buf, status);
            break;
        
        /*
         * Write Memory
         * Command Format: M addr,length:XX..
         */
        case 'M':
            ptr_next += 1;
            token_expect_integer_arg(addr);
            token_expect_seperator(',');
            token_expect_integer_arg(length);
            token_expect_seperator(':');

            /* Write Memory */
            status = dbg_mem_write(ptr_next, token_remaining_buf,
                                   addr, length, dbg_dec_hex);
            if (status == EOF) {
                goto error;
            }
            status = dbg_send_ok_packet(pkt_buf, sizeof(pkt_buf));
            break;

        /*
         * Write Memory (Binary)
         * Command Format: X addr,length:XX..
         */
        case 'X':
            ptr_next += 1;
            token_expect_integer_arg(addr);
            token_expect_seperator(',');
            token_expect_integer_arg(length);
            token_expect_seperator(':');

            /* Write Memory */
            status = dbg_mem_write(ptr_next, token_remaining_buf,
                                   addr, length, dbg_dec_bin);
            if (status == EOF) {
                goto error;
            }
            status = dbg_send_ok_packet(pkt_buf, sizeof(pkt_buf));
            break;

        /* 
         * Continue
         * Command Format: c [addr]
         */
        case 'c':
            dbg_sys_continue();
            return 0;

        /*
         * Single-step
         * Command Format: s [addr]
         */
        case 's':
            dbg_sys_step();
            return 0;

        case '?':
            status = dbg_send_signal_packet(pkt_buf, sizeof(pkt_buf), 0);
            break;

        /*
         * Unsupported Command
         */
        default:
            status = dbg_send_packet(NULL, 0);
        }

        if (status ==  EOF) {
            // Error!
            return EOF;
        }

        continue;

    error:
        dbg_send_error_packet(pkt_buf, sizeof(pkt_buf), 0x00);

        #undef token_remaining_buf
        #undef token_expect_seperator
        #undef token_expect_integer_arg
    }

    return 0;
}
