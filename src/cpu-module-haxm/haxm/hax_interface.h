/*
 * Copyright (c) 2011 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAX_INTERFACE_H_
#define HAX_INTERFACE_H_

/*
 * The interface to QEMU, notice:
 * 1) not include any file other than top level include
 * 2) will be shared by QEMU and kernel
 */

#include "hax_types.h"
#include "vcpu_state.h"

struct vmx_msr {
    uint64_t entry;
    uint64_t value;
} PACKED;

/* fx_layout has 3 formats table 3-56, 512bytes */
struct fx_layout {
    uint16  fcw;
    uint16  fsw;
    uint8   ftw;
    uint8   res1;
    uint16  fop;
    union {
        struct {
            uint32  fip;
            uint16  fcs;
            uint16  res2;
        };
        uint64  fpu_ip;
    };
    union {
        struct {
            uint32  fdp;
            uint16  fds;
            uint16  res3;
        };
        uint64  fpu_dp;
    };
    uint32  mxcsr;
    uint32  mxcsr_mask;
    uint8   st_mm[8][16];
    uint8   mmx_1[8][16];
    uint8   mmx_2[8][16];
    uint8   pad[96];
} ALIGNED(16);

/*
 * TODO: Fixed array is stupid, but it makes Mac support a bit easier, since we
 * can avoid the memory map or copyin staff. We need to fix it in future.
 */

#define HAX_MAX_MSR_ARRAY 0x20
struct hax_msr_data {
    uint16_t nr_msr;
    uint16_t done;
    uint16_t pad[2];
    struct vmx_msr entries[HAX_MAX_MSR_ARRAY];
} PACKED;

#define HAX_IO_OUT 0
#define HAX_IO_IN  1

enum exit_reason {
    INT_EXCEPTION_NMI       = 0, // An SW interrupt, exception or NMI has occurred
    EXT_INTERRUPT           = 1, // An external interrupt has occurred
    TRIPLE_FAULT            = 2, // Triple fault occurred
    INIT_EVENT              = 3,
    SIPI_EVENT              = 4,

    SMI_IO_EVENT            = 5,
    SMI_OTHER_EVENT         = 6,
    PENDING_INTERRUPT       = 7,
    PENDING_NMI             = 8,
    TASK_SWITCH             = 9,

    CPUID_INSTRUCTION       = 10, // Guest executed CPUID instruction
    GETSEC_INSTRUCTION      = 11,
    HLT_INSTRUCTION         = 12, // Guest executed HLT instruction
    INVD_INSTRUCTION        = 13, // Guest executed INVD instruction
    INVLPG_INSTRUCTION      = 14, // Guest executed INVLPG instruction
    RDPMC_INSTRUCTION       = 15, // Guest executed RDPMC instruction
    RDTSC_INSTRUCTION       = 16, // Guest executed RDTSC instruction
    RSM_INSTRUCTION         = 17,

    // Guest executed VMX instruction
    VMCALL_INSTRUCTION      = 18,
    VMCLEAR_INSTRUCTION     = 19,
    VMLAUNCH_INSTRUCTION    = 20,
    VMPTRLD_INSTRUCTION     = 21,
    VMPTRST_INSTRUCTION     = 22,
    VMREAD_INSTRUCTION      = 23,
    VMRESUME_INSTRUCTION    = 24,
    VMWRITE_INSTRUCTION     = 25,
    VMXOFF_INSTRUCTION      = 26,
    VMXON_INSTRUCTION       = 27,

    CR_ACCESS               = 28, // Guest accessed a control register
    DR_ACCESS               = 29, // Guest attempted access to debug register
    IO_INSTRUCTION          = 30, // Guest attempted io
    MSR_READ                = 31, // Guest attempted to read an MSR
    MSR_WRITE               = 32, // Guest attempted to write an MSR

    FAILED_VMENTER_GS       = 33, // VMENTER failed due to guest state
    FAILED_VMENTER_MSR      = 34, // VMENTER failed due to msr loading

    MWAIT_INSTRUCTION       = 36,
    MTF_EXIT                = 37,

    MONITOR_INSTRUCTION     = 39,
    PAUSE_INSTRUCTION       = 40,
    MACHINE_CHECK           = 41,
    TPR_BELOW_THRESHOLD     = 43,

    APIC_ACCESS             = 44,

    GDT_IDT_ACCESS          = 46,
    LDT_TR_ACCESS           = 47,

    EPT_VIOLATION           = 48,
    EPT_MISCONFIG           = 49,
    INVEPT_INSTRUCTION      = 50,
    RDTSCP_INSTRUCTION      = 51,
    VMX_TIMER_EXIT          = 52,
    INVVPID_INSTRUCTION     = 53,

    WBINVD_INSTRUCTION      = 54,
    XSETBV_INSTRUCTION      = 55,
    APIC_WRITE              = 56
};

enum exit_status {
	HAX_EXIT_IO = 1,
	HAX_EXIT_MMIO,
	HAX_EXIT_REALMODE,
	HAX_EXIT_INTERRUPT,
	HAX_EXIT_UNKNOWN,
	HAX_EXIT_HLT,
	HAX_EXIT_STATECHANGE,
	HAX_EXIT_PAUSED,
	HAX_EXIT_FAST_MMIO
};

/* The area to communicate with device model */
struct hax_tunnel {
    uint32_t _exit_reason;
    uint32_t pad0;
    uint32_t _exit_status;
    uint32_t user_event_pending;
    int ready_for_interrupt_injection;
    int request_interrupt_window;

    union {
        struct {
			uint8_t _direction;
            uint8_t _df;
            uint16_t _size;
            uint16_t _port;
            uint16_t _count;
            /* Followed owned by HAXM, QEMU should not touch them */
            /* bit 1 is 1 means string io */
            uint8_t _flags;
            uint8_t _pad0;
            uint16_t _pad1;
            uint32_t _pad2;
            vaddr_t _vaddr;
        } io;
        struct {
            paddr_t gla;
        } mmio;
        struct {
            paddr_t dummy;
        } state;
    };
} PACKED;

struct hax_fastmmio {
    paddr_t gpa;
    union {
        uint64_t value;
        paddr_t gpa2;  /* since API v4 */
    };
    uint8_t size;
    uint8_t direction;
    uint16_t reg_index;  /* obsolete */
    uint32_t pad0;
    uint64_t _cr0;
    uint64_t _cr2;
    uint64_t _cr3;
    uint64_t _cr4;
} PACKED;

struct hax_module_version {
    uint32_t compat_version;
    uint32_t cur_version;
} PACKED;

#define HAX_CAP_STATUS_WORKING     (1 << 0)
#define HAX_CAP_MEMQUOTA           (1 << 1)
#define HAX_CAP_WORKSTATUS_MASK    0x01

#define HAX_CAP_FAILREASON_VT      (1 << 0)
#define HAX_CAP_FAILREASON_NX      (1 << 1)

#define HAX_CAP_EPT                (1 << 0)
#define HAX_CAP_FASTMMIO           (1 << 1)
#define HAX_CAP_UG                 (1 << 2)
#define HAX_CAP_64BIT_RAMBLOCK     (1 << 3)
#define HAX_CAP_64BIT_SETRAM       (1 << 4)

struct hax_capabilityinfo {
    /*
     * bit 0: 1 - working, 0 - not working, possibly because NT/NX disabled
     * bit 1: 1 - memory limitation working, 0 - no memory limitation
     */
    uint16_t wstatus;
    /*
     * valid when not working
     * bit0: VT not enabeld
     * bit1: NX not enabled
     */
    /*
     * valid when working
     * bit0: EPT enabled
     * bit1: fastMMIO
     */
    uint16_t winfo;
    uint32_t win_refcount;
    uint64_t mem_quota;
} PACKED;

struct hax_tunnel_info {
    uint64_t va;
    uint64_t io_va;
    uint16_t size;
    uint16_t pad[3];
} PACKED;

struct hax_set_memlimit {
    uint8_t enable_memlimit;
    uint8_t pad[7];
    uint64_t memory_limit;
} PACKED;

struct hax_alloc_ram_info {
    uint32_t size;
    uint32_t pad;
    uint64_t va;
} PACKED;

struct hax_ramblock_info {
    uint64_t start_va;
    uint64_t size;
    uint64_t reserved;
} PACKED;

#define HAX_RAM_INFO_ROM     0x01  // read-only
#define HAX_RAM_INFO_INVALID 0x80  // unmapped, usually used for MMIO

struct hax_set_ram_info {
    uint64_t pa_start;
    uint32_t size;
    uint8_t flags;
    uint8_t pad[3];
    uint64_t va;
} PACKED;

struct hax_set_ram_info2 {
    uint64_t pa_start;
    uint64_t size;
    uint64_t va;
    uint32_t flags;
    uint32_t reserved1;
    uint64_t reserved2;
} PACKED;

/* This interface is support only after API version 2 */
struct hax_qemu_version {
    /* Current API version in QEMU*/
    uint32_t cur_version;
    /* The least API version supported by QEMU */
    uint32_t least_version;
} PACKED;

#ifdef _WIN32

//#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
//    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \

#define HAX_DEVICE_TYPE 0x4000

#define HAX_IOCTL_VERSION \
        CTL_CODE(HAX_DEVICE_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_IOCTL_CREATE_VM \
        CTL_CODE(HAX_DEVICE_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_IOCTL_CAPABILITY \
        CTL_CODE(HAX_DEVICE_TYPE, 0x910, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_IOCTL_SET_MEMLIMIT \
        CTL_CODE(HAX_DEVICE_TYPE, 0x911, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define HAX_VM_IOCTL_VCPU_CREATE \
        CTL_CODE(HAX_DEVICE_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VM_IOCTL_ALLOC_RAM \
        CTL_CODE(HAX_DEVICE_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VM_IOCTL_SET_RAM \
        CTL_CODE(HAX_DEVICE_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VM_IOCTL_VCPU_DESTROY \
        CTL_CODE(HAX_DEVICE_TYPE, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VM_IOCTL_ADD_RAMBLOCK \
        CTL_CODE(HAX_DEVICE_TYPE, 0x913, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VM_IOCTL_SET_RAM2 \
        CTL_CODE(HAX_DEVICE_TYPE, 0x914, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define HAX_VCPU_IOCTL_RUN \
        CTL_CODE(HAX_DEVICE_TYPE, 0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_IOCTL_SET_MSRS \
        CTL_CODE(HAX_DEVICE_TYPE, 0x907, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_IOCTL_GET_MSRS \
        CTL_CODE(HAX_DEVICE_TYPE, 0x908, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define HAX_VCPU_IOCTL_SET_FPU \
        CTL_CODE(HAX_DEVICE_TYPE, 0x909, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_IOCTL_GET_FPU \
        CTL_CODE(HAX_DEVICE_TYPE, 0x90a, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define HAX_VCPU_IOCTL_SETUP_TUNNEL \
        CTL_CODE(HAX_DEVICE_TYPE, 0x90b, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_IOCTL_INTERRUPT \
        CTL_CODE(HAX_DEVICE_TYPE, 0x90c, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_SET_REGS \
        CTL_CODE(HAX_DEVICE_TYPE, 0x90d, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_GET_REGS \
        CTL_CODE(HAX_DEVICE_TYPE, 0x90e, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HAX_VCPU_IOCTL_KICKOFF \
        CTL_CODE(HAX_DEVICE_TYPE, 0x90f, METHOD_BUFFERED, FILE_ANY_ACCESS)

/* API version 2.0 */
#define HAX_VM_IOCTL_NOTIFY_QEMU_VERSION \
        CTL_CODE(HAX_DEVICE_TYPE, 0x910, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif

#endif  // HAX_INTERFACE_H_
