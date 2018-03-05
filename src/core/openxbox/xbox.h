#pragma once

#include <assert.h>
#ifndef _WIN32
	#include <libgen.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openxbox/cpu.h"
#include "openxbox/dev.h"
#include "openxbox/emulator.h"
#include "openxbox/gdbserver.h"
#include "openxbox/log.h"
#include "openxbox/mem.h"
#include "openxbox/util.h"
#include "openxbox/xbe.h"
#include "openxbox/thread.h"

#include "openxbox/hw/sysclock.h"

#include "openxbox/cpu_module.h"

namespace openxbox {

/*!
 * Top-level Xbox machine class
 *
 * This class is the top-level class, will perform initialization and high-level
 * management of the overall emulation flow.
 */
class Xbox : Emulator {
protected:
	// ----- Modules ----------------------------------------------------------
	IOpenXBOXCPUModule * m_cpuModule;

	// ----- Hardware ---------------------------------------------------------
	Cpu          *m_cpu;
	char         *m_ram;
	char         *m_rom;
	MemoryRegion *m_memRegion;
	SystemClock  *m_sysClock;


	// ----- State ------------------------------------------------------------
	bool m_should_run;

	// ----- Debugger ---------------------------------------------------------
	GdbServer *m_gdb;

public:
	Xbox(IOpenXBOXCPUModule *cpuModule);
	~Xbox();
	int Initialize();

	void InitializePreRun();
	void Cleanup();

	int Run();
	int RunCpu();
	void Stop();
};

}
