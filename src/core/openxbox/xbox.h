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
#include "openxbox/settings.h"

#include "openxbox/hw/sysclock.h"
#include "openxbox/hw/smbus.h"
#include "openxbox/hw/pcibus.h"
#include "openxbox/hw/smc.h"
#include "openxbox/hw/mcpx.h"
#include "openxbox/hw/eeprom.h"
#include "openxbox/hw/nvnet.h"

#include "openxbox/cpu_module.h"

namespace openxbox {

/*!
 * Top-level Xbox machine class
 *
 * This class is the top-level class, will perform initialization and high-level
 * management of the overall emulation flow.
 */
class Xbox : Emulator, IOMapper {
protected:
	// ----- Modules ----------------------------------------------------------
	IOpenXBOXCPUModule * m_cpuModule;

	// ----- Hardware ---------------------------------------------------------
	Cpu          *m_cpu;
	char         *m_ram;
	char         *m_rom;
	MemoryRegion *m_memRegion;
	
    SystemClock  *m_sysClock;
    SMCDevice    *m_SMC;
    SMBus        *m_SMBus;
    PCIBus       *m_PCIBus;
    MCPXDevice   *m_MCPX;
    EEPROMDevice *m_EEPROM;
    NVNetDevice  *m_NVNet;

    // TODO: move to TV encoder device
    int m_field_pin = 0;

    // ----- Configuration ----------------------------------------------------
    OpenXBOXSettings *m_settings;

	// ----- State ------------------------------------------------------------
	bool m_should_run;

	// ----- Debugger ---------------------------------------------------------
	GdbServer *m_gdb;

public:
	Xbox(IOpenXBOXCPUModule *cpuModule);
	~Xbox();
	int Initialize(OpenXBOXSettings *settings);

	void InitializePreRun();
	void Cleanup();

	int Run();
	int RunCpu();
	void Stop();

    // IOMapper implementation
    void IORead(uint32_t addr, uint32_t *value, uint16_t size);
    void IOWrite(uint32_t addr, uint32_t value, uint16_t size);

    void MMIORead(uint32_t addr, uint32_t *value, uint8_t size);
    void MMIOWrite(uint32_t addr, uint32_t value, uint8_t size);
};

}
