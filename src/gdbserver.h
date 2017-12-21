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
#ifndef GDBSERVER_H
#define GDBSERVER_H

#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"

typedef uint32_t address;
typedef uint32_t reg;

enum DBG_REGISTER {
    DBG_CPU_I386_REG_EAX       = 0,
    DBG_CPU_I386_REG_ECX       = 1,
    DBG_CPU_I386_REG_EDX       = 2,
    DBG_CPU_I386_REG_EBX       = 3,
    DBG_CPU_I386_REG_ESP       = 4,
    DBG_CPU_I386_REG_EBP       = 5,
    DBG_CPU_I386_REG_ESI       = 6,
    DBG_CPU_I386_REG_EDI       = 7,
    DBG_CPU_I386_REG_PC        = 8,
    DBG_CPU_I386_REG_PS        = 9,
    DBG_CPU_I386_REG_CS        = 10,
    DBG_CPU_I386_REG_SS        = 11,
    DBG_CPU_I386_REG_DS        = 12,
    DBG_CPU_I386_REG_ES        = 13,
    DBG_CPU_I386_REG_FS        = 14,
    DBG_CPU_I386_REG_GS        = 15,
    DBG_CPU_I386_NUM_REGISTERS = 16
};

struct dbg_state {
    int signum;
    reg registers[DBG_CPU_I386_NUM_REGISTERS];
};

typedef int (*dbg_enc_func)(char *buf, size_t buf_len, const char *data, size_t data_len);
typedef int (*dbg_dec_func)(const char *buf, size_t buf_len, char *data, size_t data_len);

class GdbServer {
protected:
    Cpu                *m_cpu;
    const char         *m_bind_host;
    int                 m_bind_port;
    struct sockaddr_in  m_bind_addr;
    struct sockaddr_in  m_peer_addr;
    socklen_t           m_peer_addr_len;
    int                 m_sockfd, m_peer_sockfd;
    struct dbg_state    m_dbg_state;

    int dbg_sys_getc(void);
    int dbg_sys_putchar(int ch);
    int dbg_sys_mem_readb(address addr, char *val);
    int dbg_sys_mem_writeb(address addr, char val);
    int dbg_sys_continue();
    int dbg_sys_step();

    // Communication functions
    int dbg_write(const char *buf, size_t len);
    int dbg_read(char *buf, size_t buf_len, size_t len);

    // String processing helper functions
    static int dbg_is_printable_char(char ch);
    static char dbg_get_digit(int val);
    static int dbg_get_val(char digit, int base);
    static int dbg_strtol(const char *str, size_t len, int base, const char **endptr);

    // Data encoding/decoding
    static int dbg_enc_hex(char *buf, size_t buf_len, const char *data, size_t data_len);
    static int dbg_dec_hex(const char *buf, size_t buf_len, char *data, size_t data_len);
    static int dbg_enc_bin(char *buf, size_t buf_len, const char *data, size_t data_len);
    static int dbg_dec_bin(const char *buf, size_t buf_len, char *data, size_t data_len);

    // Packet functions
    int dbg_send_packet(const char *pkt, size_t pkt_len);
    int dbg_recv_packet(char *pkt_buf, size_t pkt_buf_len, size_t *pkt_len);
    int dbg_checksum(const char *buf, size_t len);
    int dbg_recv_ack(void);

    // Packet creation helpers
    int dbg_send_ok_packet(char *buf, size_t buf_len);
    int dbg_send_conmsg_packet(char *buf, size_t buf_len, const char *msg);
    int dbg_send_signal_packet(char *buf, size_t buf_len, char signal);
    int dbg_send_error_packet(char *buf, size_t buf_len, char error);

    // Command functions
    int dbg_mem_read(char *buf, size_t buf_len, address addr, size_t len, dbg_enc_func enc);
    int dbg_mem_write(const char *buf, size_t buf_len, address addr, size_t len, dbg_dec_func dec);

    int dbg_main();

public:
    GdbServer(Cpu *cpu, const char *bind_host, int bind_port);
    ~GdbServer();
    int Initialize();
    int WaitForConnection();
    int Debug(int signal);
    int Shutdown();
};

#endif
