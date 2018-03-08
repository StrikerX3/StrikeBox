#include "openxbox/settings.h"
#include "openxbox/xbox.h"
#include "openxbox/timer.h"
#include "openxbox/alloc.h"
#include "openxbox/debug.h"

#include "openxbox/hw/defs.h"
#include "openxbox/hw/tvenc.h"

#include <chrono>

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

    // ----- RAM --------------------------------------------------------------

    // Create RAM region
    log_debug("Allocating RAM (%d MiB)\n", XBOX_RAM_SIZE >> 20);
    m_ram = (char *)valloc(XBOX_RAM_SIZE);
    assert(m_ram != NULL);
    memset(m_ram, 0, XBOX_RAM_SIZE);

    // Map RAM at address 0x00000000
    rgn = new MemoryRegion(MEM_REGION_RAM, 0x00000000, XBOX_RAM_SIZE, m_ram);
    assert(rgn != NULL);
    m_memRegion->AddSubRegion(rgn);

    // ----- ROM --------------------------------------------------------------

    // Create ROM region
    log_debug("Allocating ROM (%d MiB)\n", XBOX_ROM_SIZE >> 20);
    m_rom = (char *)valloc(XBOX_ROM_SIZE);
    assert(m_rom != NULL);
    memset(m_rom, 0, XBOX_ROM_SIZE);

    // Map ROM to address 0xFF000000
    rgn = new MemoryRegion(MEM_REGION_ROM, 0xFF000000, XBOX_ROM_SIZE, m_rom);
    assert(rgn != NULL);
    m_memRegion->AddSubRegion(rgn);

    // Load ROM files
    FILE *fp;
    errno_t e;
    long sz;

    // Load MCPX ROM
    log_debug("Loading MCPX ROM %s...", settings->rom_mcpx);
    e = fopen_s(&fp, settings->rom_mcpx, "rb");
    if (e) {
        log_debug("file %s could not be opened\n", settings->rom_mcpx);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz != 512) {
        log_debug("incorrect file size: %d (must be 512 bytes)\n", sz);
        return 2;
    }
    char *mcpx = new char[sz];
    fread_s(mcpx, sz, 1, sz, fp);
    fclose(fp);
    log_debug("OK\n");

    // Load BIOS ROM
    log_debug("Loading BIOS ROM %s...", settings->rom_bios);
    e = fopen_s(&fp, settings->rom_bios, "rb");
    if (e) {
        log_debug("file %s could not be opened\n", settings->rom_bios);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz != KiB(256) && sz != MiB(1)) {
        log_debug("incorrect file size: %d (must be 256 KiB or 1024 KiB)\n", sz);
        return 3;
    }
    char *bios = new char[sz];
    fread_s(bios, sz, 1, sz, fp);
    fclose(fp);
    log_debug("OK (%d KiB)\n", sz >> 10);

    uint32_t biosSize = sz;

    // Load BIOS ROM image
    memcpy(m_rom, bios, biosSize);

    // Overlay MCPX ROM image onto the last 512 bytes
    memcpy(m_rom + biosSize - 512, mcpx, 512);

    // Replicate resulting ROM image across the entire 16 MiB range
    for (uint32_t addr = biosSize; addr < MiB(16); addr += biosSize) {
        memcpy(m_rom + addr, m_rom, biosSize);
    }

    // ----- CPU --------------------------------------------------------------

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
    m_cpu->Initialize(this);

    // Allow CPU to update memory map based on device allocation, etc
    m_cpu->MemMap(m_memRegion);

    // ----- Hardware ---------------------------------------------------------

    // Initialize system clock
    log_debug("Initializing System Clock\n");
    m_sysClock = new SystemClock(m_cpu, settings->hw_sysclock_tickRate);
    assert(m_sysClock != NULL);

    // Determine which revisions of which components should be used for the
    // specified hardware model
    MCPXRevision mcpxRevision = MCPXRevisionFromHardwareModel(settings->hw_model);
    SMCRevision smcRevision = SMCRevisionFromHardwareModel(settings->hw_model);
    TVEncoder tvEncoder = TVEncoderFromHardwareModel(settings->hw_model);

    log_debug("Initializing devices\n");
    // Create busses
    m_PCIBus = new PCIBus();
    m_SMBus = new SMBus();

    // Create devices
    m_MCPX = new MCPXDevice(mcpxRevision);
    m_SMC = new SMCDevice(smcRevision);
    m_EEPROM = new EEPROMDevice();
    m_NVNet = new NVNetDevice();
    // TODO: m_NV2A = new NV2ADevice();

    // Connect devices to SMBus
    m_SMBus->ConnectDevice(kSMBusAddress_SystemMicroController, m_SMC); // W 0x20 R 0x21
    m_SMBus->ConnectDevice(kSMBusAddress_EEPROM, m_EEPROM); // W 0xA8 R 0xA9

    // TODO: Other SMBus devices to connect
    //m_SMBus->ConnectDevice(kSMBusAddress_MCPX, m_MCPX); // W 0x10 R 0x11 -- TODO : Is MCPX an SMBus and/or PCI device?
    //m_SMBus->ConnectDevice(kSMBusAddress_TemperatureMeasurement, m_TemperatureMeasurement); // W 0x98 R 0x99
    //m_SMBus->ConnectDevice(kSMBusAddress_TVEncoder, m_TVEncoder); // W 0x88 R 0x89
    switch (tvEncoder) {
    case TVEncoder::Conexant:
        // g_SMBus->ConnectDevice(kSMBusAddress_TVEncoder_ID_Conexant, m_TVEncoderConexant); // W 0x8A R 0x8B
        break;
    case TVEncoder::Focus:
        // g_SMBus->ConnectDevice(kSMBusAddress_TVEncoder_ID_Focus, m_TVEncoderFocus); // W 0xD4 R 0xD5
        break;
    case TVEncoder::XCalibur:
        // g_SMBus->ConnectDevice(kSMBusAddress_TVEncoder_ID_XCalibur, m_TVEncoderXCalibur); // W 0xE0 R 0xE1
        break;
    }

    // Connect devices to PCI bus
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 1)), m_SMBus);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 0)), m_NVNet);
    //m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 1)), m_MCPX); // MCPX device ID = 0x0808 ?
    //m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(5, 0)), m_NVAPU);
    //m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(6, 0)), m_AC97);
    //m_PCIBus->ConnectDevice(PCI_DEVID(1, PCI_DEVFN(0, 0)), m_NV2A);

    // TODO: Handle other SMBUS Addresses, like PIC_ADDRESS, XCALIBUR_ADDRESS
    // Resources:
    // http://pablot.com/misc/fancontroller.cpp
    // https://github.com/JayFoxRox/Chihiro-Launcher/blob/master/hook.h
    // https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
    // https://web.archive.org/web/20100617022549/http://www.xbox-linux.org/wiki/PIC

    // ----- Debugger ---------------------------------------------------------

    // GDB Server
    if (m_settings->gdb_enable) {
        m_gdb = new GdbServer(m_cpu, "127.0.0.1", 9269);
        m_gdb->Initialize();
    }

    return 0;
}

void Xbox::InitializePreRun() {
    if (m_settings->gdb_enable) {
        // Allow debugging before running so client can setup breakpoints, etc
        log_debug("Starting GDB Server\n");
        m_gdb->WaitForConnection();
        m_gdb->Debug(1);
    }
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
#if 0
#ifdef _DEBUG
        t.Start();
#endif
#endif
        if (m_settings->cpu_singleStep) {
            result = m_cpu->Step();
        }
        else {
            result = m_cpu->Run();
        }
#if 0
#ifdef _DEBUG
        t.Stop();
		log_debug("CPU Executed for %lld ms\n", t.GetMillisecondsElapsed());
#endif
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

#if 1
#ifdef _DEBUG
        // Pring CPU registers for debugging purposes
        uint32_t eip;
        m_cpu->RegRead(REG_EIP, &eip);
        DumpCPURegisters(m_cpu);
#endif
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

void Xbox::IORead(uint32_t addr, uint32_t *value, uint16_t size) {
    switch (addr) {
    case 0x8008: { // TODO: Move 0x8008 TIMER to a device
        if (size == sizeof(uint32_t)) {
            // This timer counts at 3579545 Hz
            auto t = std::chrono::high_resolution_clock::now();
            *value = static_cast<uint32_t>(t.time_since_epoch().count() * 0.003579545);
        }
        break;
    }
    case 0x80C0: { // TODO: Move 0x80C0 TV encoder to a device
        if (size == sizeof(uint8_t)) {
            // field pin from tv encoder?
            m_field_pin = (m_field_pin + 1) & 1;
            *value = m_field_pin << 5;
        }
        break;
    }
    }

    // Pass the IO Read to the PCI Bus.
    // This will handle devices with BARs set to IO addresses
    if (m_PCIBus->IORead(addr, value, size)) {
        return;
    }

    log_warning("Unhandled I/O read!   address = 0x%08x,  size = %d\n", addr, size);
}

void Xbox::IOWrite(uint32_t addr, uint32_t value, uint16_t size) {
    // Pass the IO Write to the PCI Bus.
    // This will handle devices with BARs set to IO addresses
    if (m_PCIBus->IOWrite(addr, value, size)) {
        return;
    }

    log_warning("Unhandled I/O write!  address = 0x%08x,  size = %d,  value = 0x%08x\n", addr, size, value);
}

void Xbox::MMIORead(uint32_t addr, uint32_t *value, uint8_t size) {
    if ((addr & (size - 1)) != 0) {
        log_warning("Unaligned MMIO read!   address = 0x%08x,  size = %d\n", addr, size);
        return;
    }

    // Pass the read to the PCI Bus.
    // This will handle devices with BARs set to MMIO addresses
    if (m_PCIBus->MMIORead(addr, value, size)) {
        return;
    }

    log_debug("Unhandled MMIO read!   address = 0x%08x,  size = %d\n", addr, size);
}

void Xbox::MMIOWrite(uint32_t addr, uint32_t value, uint8_t size) {
    if ((addr & (size - 1)) != 0) {
        log_warning("Unaligned MMIO write!  address = 0x%08x,  size = %d,  value = 0x%08x\n", addr, size, value);
        return;
    }

    // Pass the write to the PCI Bus.
    // This will handle devices with BARs set to MMIO addresses
    if (m_PCIBus->MMIOWrite(addr, value, size)) {
        return;
    }

    
    log_debug("Unhandled MMIO write!  address = 0x%08x,  size = %d,  value = 0x%08x\n", addr, size, value);
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