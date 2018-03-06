#include "openxbox/settings.h"
#include "openxbox/xbox.h"
#include "openxbox/timer.h"
#include "openxbox/alloc.h"
#include "openxbox/debug.h"

namespace openxbox {

// CPU emulation thread function
static uint32_t EmuCpuThreadFunc(void *data) {
	Xbox *xbox = (Xbox *)data;
	return xbox->RunCpu();
}

// System clock thread function
static uint32_t EmuSysClockThreadFunc(void *data) {
	SystemClock *sysClock = (SystemClock *)data;
	sysClock->Run();
	return 0;
}


/*!
 * Constructor
 */
Xbox::Xbox(IOpenXBOXCPUModule *cpuModule)
	: m_cpuModule(cpuModule)
{
	m_sysClock = nullptr;
}

/*!
 * Destructor
 */
Xbox::~Xbox()
{
	if (m_cpu) m_cpuModule->FreeCPU(m_cpu);
	if (m_ram) vfree(m_ram);
	if (m_rom) vfree(m_rom);
	if (m_memRegion) delete m_memRegion;
}

/*!
 * Perform basic system initialization
 */
int Xbox::Initialize(OpenXBOXSettings *settings)
{
    m_settings = settings;
	MemoryRegion *rgn;

	// Initialize 4 GiB address space
	m_memRegion = new MemoryRegion(MEM_REGION_NONE, 0x00000000, 0x100000000ULL, NULL);

	// Create RAM region
	log_debug("Allocating RAM (%d MiB)\n", XBOX_RAM_SIZE >> 20);
	m_ram = (char *)valloc(XBOX_RAM_SIZE);
	assert(m_ram != NULL);
	memset(m_ram, 0, XBOX_RAM_SIZE);

	// Map RAM at address 0x00000000
	rgn = new MemoryRegion(MEM_REGION_RAM, 0x00000000, XBOX_RAM_SIZE, m_ram);
	assert(rgn != NULL);
	m_memRegion->AddSubRegion(rgn);

	// Create ROM region
	log_debug("Allocating ROM (%d KiB)\n", XBOX_ROM_SIZE >> 10);
	m_rom = (char *)valloc(XBOX_ROM_SIZE);
	assert(m_rom != NULL);
	memset(m_rom, 0, XBOX_ROM_SIZE);

    // Map ROM to address 0xFF000000
    rgn = new MemoryRegion(MEM_REGION_ROM, 0xFF000000, XBOX_ROM_SIZE, m_rom);
    assert(rgn != NULL);
    m_memRegion->AddSubRegion(rgn);
    
	// Initialize CPU
	log_debug("Initializing CPU\n");
	if (m_cpuModule == nullptr) {
		log_fatal("No CPU module specified\n");
		return -1;
	}
	m_cpu = m_cpuModule->GetCPU();
	if (m_cpu == nullptr) {
		log_fatal("CPU instantiation failed\n");
		return -1;
	}
	m_cpu->Initialize();

	// Allow CPU to update memory map based on device allocation, etc
	m_cpu->MemMap(m_memRegion);

	// Initialize system clock
	log_debug("Initializing System Clock\n");
	m_sysClock = new SystemClock(m_cpu, 100.0f); // TODO: configurable tick rate
	assert(m_sysClock != NULL);

	// GDB Server
    if (m_settings->gdb_enable) {
        log_debug("Starting GDB Server\n");
        m_gdb = new GdbServer(m_cpu, "127.0.0.1", 9269);
        m_gdb->Initialize();
    }

	return 0;
}

void Xbox::LoadROMs(char *mcpx, char *bios, uint32_t biosSize) {
    // Load BIOS ROM image
    memcpy(m_rom, bios, biosSize);

    // Overlay MCPX ROM image onto the last 512 bytes
    memcpy(m_rom + biosSize - 512, mcpx, 512);

    // Replicate resulting ROM image across the entire 16 MiB range
    for (uint32_t addr = biosSize; addr < MiB(16); addr += biosSize) {
        memcpy(m_rom + addr, m_rom, biosSize);
    }
}

void Xbox::InitializePreRun() {
#if ENABLE_GDB_SERVER
	// Allow debugging before running so client can setup breakpoints, etc
	m_gdb->WaitForConnection();
	m_gdb->Debug(1);
#endif
}

int Xbox::Run() {
	m_should_run = true;

	// --- CPU emulation ------------------------------------------------------

	// Start CPU emulation on a new thread
	Thread *cpuIdleThread = Thread_Create("[HW] CPU", EmuCpuThreadFunc, this);

	// --- Emulator subsystems ------------------------------------------------

	// Start System Clock thread
	Thread_Create("[HW] System clock", EmuSysClockThreadFunc, m_sysClock);
	
	// TODO: start threads to handle other subsystems
	// - One or more for each piece of hardware
	//   - NV2A
	//   - MCPX (multiple components)

	return Thread_Join(cpuIdleThread);
}

/*!
 * Advances the CPU emulation state.
 */
int Xbox::RunCpu()
{
	Timer t;
	int result;
	struct CpuExitInfo *exit_info;

	if (!m_should_run) {
		Thread_Exit(-1);
	}

	while (m_should_run) {
		// Run CPU emulation
#ifdef _DEBUG
        t.Start();
#endif
		result = m_cpu->Run(100 * 1000);
#ifdef _DEBUG
        t.Stop();
		log_debug("CPU Executed for %lld ms\n", t.GetMillisecondsElapsed());
#endif

		// Handle result
		if (result != 0) {
			log_error("Error occurred!\n");
			if (LOG_LEVEL >= LOG_LEVEL_DEBUG) {
				uint32_t eip;
				m_cpu->RegRead(REG_EIP, &eip);
				DumpCPURegisters(m_cpu);
				DumpCPUStack(m_cpu);
				DumpCPUMemory(m_cpu, eip, 0x40, true);
				DumpCPUMemory(m_cpu, eip, 0x40, false);
			}
			// Stop emulation
			Stop();
			break;
		}

        // Pring CPU registers for debugging purposes
#ifdef _DEBUG
            uint32_t eip;
            m_cpu->RegRead(REG_EIP, &eip);
            DumpCPURegisters(m_cpu);
#endif

		// Handle reason for the CPU to exit
		exit_info = m_cpu->GetExitInfo();
		switch (exit_info->reason) {
		case CPU_EXIT_SHUTDOWN: log_info("VM is shutting down\n"); Stop(); break;
		}
	}

	return result;
}

void Xbox::Stop() {
	if (m_sysClock != nullptr) {
		m_sysClock->Stop();
	}
	m_should_run = false;
}

void Xbox::Cleanup() {
	if (LOG_LEVEL >= LOG_LEVEL_DEBUG) {
		log_debug("CPU state at the end of execution:\n");
		uint32_t eip;
		m_cpu->RegRead(REG_EIP, &eip);
		DumpCPURegisters(m_cpu);
		DumpCPUStack(m_cpu);
		DumpCPUMemory(m_cpu, eip, 0x80, true);
		DumpCPUMemory(m_cpu, eip, 0x80, false);

		auto skippedInterrupts = m_cpu->GetSkippedInterrupts();
		for (uint8_t i = 0; i < 0x40; i++) {
			if (skippedInterrupts[i] > 0) {
				log_warning("Interrupt vector 0x%02x: %d interrupt requests were skipped\n", i, skippedInterrupts[i]);
			}
		}

        if (m_settings->debug_dumpMemoryMapping) {
            uint32_t cr0;
            m_cpu->RegRead(REG_CR0, &cr0);
            if (cr0 & (CR0_PG | CR0_PE)) {
                log_debug("\nVirtual address mappings:\n");
                uint32_t cr3;
                m_cpu->RegRead(REG_CR3, &cr3);
                for (uint32_t pdeEntry = 0; pdeEntry < 0x1000; pdeEntry += sizeof(Pte)) {
                    Pte *pde;
                    uint32_t pdeAddr = cr3 + pdeEntry;
                    pde = (Pte *)&m_ram[pdeAddr];

                    char pdeFlags[] = "-----------";
                    pdeFlags[0] = pde->persistAllocation ? 'P' : '-';
                    pdeFlags[1] = pde->guardOrEndOfAllocation ? 'E' : '-';
                    pdeFlags[2] = pde->global ? 'G' : '-';
                    pdeFlags[3] = pde->largePage ? 'L' : '-';
                    pdeFlags[4] = pde->dirty ? 'D' : '-';
                    pdeFlags[5] = pde->accessed ? 'A' : '-';
                    pdeFlags[6] = pde->cacheDisable ? 'N' : '-';
                    pdeFlags[7] = pde->writeThrough ? 'T' : '-';
                    pdeFlags[8] = pde->owner ? 'U' : 'K';
                    pdeFlags[9] = pde->write ? 'W' : 'R';
                    pdeFlags[10] = pde->valid ? 'V' : '-';

                    if (pde->largePage) {
                        uint32_t vaddr = (pdeEntry << 20);
                        uint32_t paddr = (pde->pageFrameNumber << 12);

                        log_debug("  0x%08x..0x%08x  ->  0x%08x..0x%08x   PDE 0x%08x [%s]\n",
                            vaddr, vaddr + MiB(4) - 1,
                            paddr, paddr + MiB(4) - 1,
                            pdeAddr, pdeFlags);
                    }
                    else if (pde->valid) {
                        for (uint32_t pteEntry = 0; pteEntry < 0x1000; pteEntry += sizeof(Pte)) {
                            Pte *pte;
                            uint32_t pteAddr = (pde->pageFrameNumber << 12) + pteEntry;
                            pte = (Pte *)&m_ram[pteAddr];

                            char pteFlags[] = "----------";
                            pteFlags[0] = pte->persistAllocation ? 'P' : '-';
                            pteFlags[1] = pte->guardOrEndOfAllocation ? 'E' : '-';
                            pteFlags[2] = pte->global ? 'G' : '-';
                            pteFlags[3] = pte->dirty ? 'D' : '-';
                            pteFlags[4] = pte->accessed ? 'A' : '-';
                            pteFlags[5] = pte->cacheDisable ? 'N' : '-';
                            pteFlags[6] = pte->writeThrough ? 'T' : '-';
                            pteFlags[7] = pte->owner ? 'U' : 'K';
                            pteFlags[8] = pte->write ? 'W' : 'R';
                            pteFlags[9] = pte->valid ? 'V' : '-';

                            if (pte->valid) {
                                uint32_t vaddr = (pdeEntry << 20) | (pteEntry << 10);
                                uint32_t paddr = (pte->pageFrameNumber << 12) | (vaddr & (KiB(4) - 1));
                                log_debug("  0x%08x..0x%08x  ->  0x%08x..0x%08x   PDE 0x%08x [%s]   PTE 0x%08x [%s]\n",
                                    vaddr, vaddr + KiB(4) - 1,
                                    paddr, paddr + KiB(4) - 1,
                                    pdeAddr, pdeFlags,
                                    pteAddr, pteFlags);
                            }
                        }
                    }
                }
            }
        }
	}

    if (m_settings->gdb_enable) {
        m_gdb->Shutdown();
    }
}

}