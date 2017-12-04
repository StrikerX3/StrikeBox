#ifndef XBOX_H
#define XBOX_H

#include <assert.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "dev.h"
#include "log.h"
#include "mem.h"
#include "sched.h"
#include "util.h"
#include "xbe.h"
#include "video/video.h"

#include "kernel/types.h"
#include "kernel/imports.h"
#include "kernel/vars.h"

#ifdef CPU_UNICORN
#include "cpu_uc.h"
#endif

/*!
 * Top-level Xbox machine class
 * 
 * This class is the top-level class, will perform initialization and high-level
 * management of the overall emulation flow.
 */
class Xbox {
protected:
    Scheduler    *m_sched;
    MemoryRegion *m_mem;
    Cpu          *m_cpu;
    Xbe          *m_xbe;
    char         *m_ram;
    Video        *m_video;
    uint32_t      m_kthunk_table_size;
    uint32_t      m_kthunk_table_base;
    uint32_t      m_import_addrs[379];
    bool          m_should_run;

    struct KernelVariables *m_kvars; // FIXME: structure isn't correct right

public:
    Xbox();
    ~Xbox();
    int Initialize();
    int LoadXbe(Xbe *xbe);
    uint32_t UnscrambleAddress(uint32_t addr, uint32_t debug, uint32_t retail);
    int Run();
    int HandleKernelEntry();
	void KernelFunctionNotImplemented();

	// Generate kernel method prototypes
	#define KERNEL_IMPORT_NULL(ID)
	#define KERNEL_IMPORT_DATA(ID, NAME)
	#define KERNEL_IMPORT_FUNC(ID, NAME) int NAME ();
	KERNEL_IMPORTS
	#undef KERNEL_IMPORT_NULL
	#undef KERNEL_IMPORT_DATA
	#undef KERNEL_IMPORT_FUNC
};

#endif
