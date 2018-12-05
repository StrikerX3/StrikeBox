#include "vixen/xbox.h"
#include "vixen/timer.h"
#include "vixen/alloc.h"
#include "vixen/debug.h"
#include "vixen/settings.h"

#include "vixen/hw/defs.h"
#include "vixen/hw/sm/tvenc.h"
#include "vixen/hw/sm/tvenc_conexant.h"

#include "vixen/hw/basic/char_null.h"
#ifdef _WIN32
#include "vixen/hw/basic/win32/char_serial.h"
#endif

#include "vixen/hw/ata/drvs/drv_dummy_hd.h"

#ifdef __linux__
#include <sys/mman.h>
#endif

#include "Zydis/Zydis.h"

#include <chrono>

namespace vixen {

// bunnie's EEPROM
const static uint8_t kDefaultEEPROM[] = {
    0xe3, 0x1c, 0x5c, 0x23, 0x6a, 0x58, 0x68, 0x37,
    0xb7, 0x12, 0x26, 0x6c, 0x99, 0x11, 0x30, 0xd1,
    0xe2, 0x3e, 0x4d, 0x56, 0xf7, 0x73, 0x2b, 0x73,
    0x85, 0xfe, 0x7f, 0x0a, 0x08, 0xef, 0x15, 0x3c,
    0x77, 0xee, 0x6d, 0x4e, 0x93, 0x2f, 0x28, 0xee,
    0xf8, 0x61, 0xf7, 0x94, 0x17, 0x1f, 0xfc, 0x11,
    0x0b, 0x84, 0x44, 0xed, 0x31, 0x30, 0x35, 0x35,
    0x38, 0x31, 0x31, 0x31, 0x34, 0x30, 0x30, 0x33,
    0x00, 0x50, 0xf2, 0x4f, 0x65, 0x52, 0x00, 0x00,
    0x0a, 0x1e, 0x35, 0x33, 0x71, 0x85, 0x31, 0x4d,
    0x59, 0x12, 0x38, 0x48, 0x1c, 0x91, 0x53, 0x60,
    0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x75, 0x61, 0x57, 0xfb, 0x2c, 0x01, 0x00, 0x00,
    0x45, 0x53, 0x54, 0x00, 0x45, 0x44, 0x54, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0a, 0x05, 0x00, 0x02, 0x04, 0x01, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc4, 0xff, 0xff, 0xff,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/*!
 * Constructor
 */
Xbox::Xbox(vixen::modules::cpu::ICPUModule *cpuModule)
    : m_cpuModule(cpuModule)
{
}

/*!
 * Destructor
 */
Xbox::~Xbox() {
    if (m_cpu) m_cpuModule->FreeCPU(m_cpu);
    if (m_ram) {
#ifdef _WIN32
        vfree(m_ram);
#endif
#ifdef __linux__
        munmap(m_ram, m_ramSize);
#endif
    }
    if (m_rom) {
#ifdef _WIN32
        vfree(m_rom);
#endif
#ifdef __linux__
        munmap(m_rom, XBOX_ROM_AREA_SIZE);
#endif
    }
    if (m_bios != nullptr) delete[] m_bios;
    if (m_mcpxROM != nullptr) delete[] m_mcpxROM;
    if (m_memRegion) delete m_memRegion;

    if (m_SMC != nullptr) delete m_SMC;
    if (m_EEPROM != nullptr) delete m_EEPROM;
    if (m_TVEncoder != nullptr) delete m_TVEncoder;
    
    if (m_SMBus != nullptr) delete m_SMBus;
    
    if (m_MCPXRAM != nullptr) delete m_MCPXRAM;
    if (m_LPC != nullptr) delete m_LPC;
    if (m_USB1 != nullptr) delete m_USB1;
    if (m_USB2 != nullptr) delete m_USB2;
    if (m_NVNet != nullptr) delete m_NVNet;
    if (m_NVAPU != nullptr) delete m_NVAPU;
    if (m_AC97 != nullptr) delete m_AC97;
    if (m_BMIDE != nullptr) delete m_BMIDE;
    if (m_NV2A != nullptr) delete m_NV2A;
    
    if (m_PCIBus != nullptr) delete m_PCIBus;
    
    if (m_HostBridge != nullptr) delete m_HostBridge;
    if (m_PCIBridge != nullptr) delete m_PCIBridge;
    if (m_AGPBridge != nullptr) delete m_AGPBridge;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            delete m_ataDrivers[i][j];
        }
    }
    if (m_ATA != nullptr) delete m_ATA;
    if (m_SuperIO != nullptr) delete m_SuperIO;
    if (m_i8254 != nullptr) delete m_i8254;
    if (m_i8259 != nullptr) delete m_i8259;
    if (m_CMOS != nullptr) delete m_CMOS;
    if (m_acpiIRQs != nullptr) delete[] m_acpiIRQs;
    if (m_IRQs != nullptr) delete[] m_IRQs;
    if (m_GSI != nullptr) delete m_GSI;
}

void Xbox::CopySettings(viXenSettings *settings) {
    m_settings = *settings;
}

EmulatorStatus Xbox::Run() {
    EmulatorStatus status = Initialize();
    if (status != EMUS_OK) {
        return status;
    }

    m_should_run = true;

    // Start CPU emulation on a new thread
    uint32_t result;
    std::thread cpuIdleThread([&] { result = EmuCpuThreadFunc(this); });

    // Wait for the thread to exit
    cpuIdleThread.join();

    Cleanup();

    return EMUS_OK;
}

void Xbox::Stop() {
    if (m_i8254 != nullptr) {
        m_i8254->Reset();
    }
    m_should_run = false;
}

/*!
 * Perform basic system initialization
 */
EmulatorStatus Xbox::Initialize()
{
    EmulatorStatus result;
    result = InitFixupSettings(); if (result != EMUS_OK) return result;
    result = InitMemory(); if (result != EMUS_OK) return result;
    result = InitCPU(); if (result != EMUS_OK) return result;
    result = InitHardware(); if (result != EMUS_OK) return result;
    result = InitDebugger(); if (result != EMUS_OK) return result;

    return EMUS_OK;
}

EmulatorStatus Xbox::InitFixupSettings() {
    if (m_settings.hw_model == DebugKit) {
        m_settings.hw_enableSuperIO = true;
        m_settings.ram_expanded = true;
    }

    return EMUS_OK;
}

EmulatorStatus Xbox::InitMemory() {
    // Initialize 4 GiB address space
    m_memRegion = new MemoryRegion(MEM_REGION_NONE, 0x00000000, 0x100000000ULL, NULL);
    if (m_memRegion == nullptr) {
        return EMUS_INIT_ALLOC_MEM_RGN_FAILED;
    }

    EmulatorStatus result;
    result = InitRAM(); if (result != EMUS_OK) return result;
    result = InitROM(); if (result != EMUS_OK) return result;

    return EMUS_OK;
}

EmulatorStatus Xbox::InitRAM() {
    // Create RAM region
    m_ramSize = m_settings.ram_expanded ? XBOX_RAM_SIZE_DEBUG : XBOX_RAM_SIZE_RETAIL;
    log_debug("Allocating RAM (%d MiB)\n", m_ramSize >> 20);

#ifdef _WIN32
    m_ram = (uint8_t *)valloc(m_ramSize);
#endif

#ifdef __linux__
    m_ram = (uint8_t *)mmap(nullptr, m_ramSize, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
#endif

    if (m_ram == NULL) {
        return EMUS_INIT_ALLOC_RAM_FAILED;
    }
    memset(m_ram, 0, m_ramSize);

    // Map RAM at address 0x00000000
    MemoryRegion *rgn = new MemoryRegion(MEM_REGION_RAM, 0x00000000, m_ramSize, m_ram);
    if (rgn == nullptr) {
        return EMUS_INIT_ALLOC_RAM_RGN_FAILED;
    }
    m_memRegion->AddSubRegion(rgn);

    return EMUS_OK;
}

EmulatorStatus Xbox::InitROM() {
    // Create ROM region
    log_debug("Allocating ROM (%d MiB)\n", XBOX_ROM_AREA_SIZE >> 20);

#ifdef _WIN32
    m_rom = (uint8_t *)valloc(XBOX_ROM_AREA_SIZE);
#endif

#ifdef __linux__
    m_rom = (uint8_t *)mmap(nullptr, XBOX_ROM_AREA_SIZE, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
#endif

    if (m_rom == NULL) {
        return EMUS_INIT_ALLOC_ROM_FAILED;
    }
    memset(m_rom, 0, XBOX_ROM_AREA_SIZE);

    // Map ROM to address 0xFF000000
    MemoryRegion *rgn = new MemoryRegion(MEM_REGION_ROM, 0xFF000000, XBOX_ROM_AREA_SIZE, m_rom);
    if (rgn == nullptr) {
        return EMUS_INIT_ALLOC_ROM_RGN_FAILED;
    }
    m_memRegion->AddSubRegion(rgn);

    // Load ROM files
    FILE *fp;
    long sz;

    // Load MCPX ROM
    log_debug("Loading MCPX ROM %s.. .", m_settings.rom_mcpx);
    fp = fopen(m_settings.rom_mcpx, "rb");
    if (fp == NULL) {
        log_debug("file %s could not be opened\n", m_settings.rom_mcpx);
        return EMUS_INIT_MCPX_ROM_NOT_FOUND;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz != 512) {
        log_debug("incorrect file size: %d (must be 512 bytes)\n", sz);
        return EMUS_INIT_MCPX_ROM_INVALID_SIZE;
    }
    m_mcpxROM = new uint8_t[sz];
    fread(m_mcpxROM, 1, sz, fp);
    fclose(fp);
    log_debug("OK\n");

    // Load BIOS ROM
    log_debug("Loading BIOS ROM %s... ", m_settings.rom_bios);
    fp = fopen(m_settings.rom_bios, "rb");
    if (fp == NULL) {
        log_debug("file %s could not be opened\n", m_settings.rom_bios);
        return EMUS_INIT_BIOS_ROM_NOT_FOUND;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz != KiB(256) && sz != MiB(1)) {
        log_debug("incorrect file size: %d (must be 256 KiB or 1024 KiB)\n", sz);
        return EMUS_INIT_BIOS_ROM_INVALID_SIZE;
    }
    m_bios = new uint8_t[sz];
    fread(m_bios, 1, sz, fp);
    fclose(fp);
    log_debug("OK (%d KiB)\n", sz >> 10);

    m_biosSize = sz;

    return EMUS_OK;
}

EmulatorStatus Xbox::InitCPU() {
    log_debug("Initializing CPU\n");
    if (m_cpuModule == nullptr) {
        log_fatal("No CPU module specified\n");
        return EMUS_INIT_NO_CPU_MODULE;
    }
    m_cpu = m_cpuModule->GetCPU();
    if (m_cpu == nullptr) {
        log_fatal("CPU instantiation failed\n");
        return EMUS_INIT_CPU_CREATE_FAILED;
    }
    if (m_cpu->Initialize(&m_ioMapper)) {
        log_fatal("CPU initialization failed\n");
        return EMUS_INIT_CPU_INIT_FAILED;
    }

    // Allow CPU to update memory map
    auto result = m_cpu->MemMap(m_memRegion);
    if (result != CPUS_MMAP_OK) {
        return EMUS_INIT_CPU_MEM_MAP_FAILED;
    }

    return EMUS_OK;
}

EmulatorStatus Xbox::InitHardware() {
    // Determine which revisions of which components should be used for the
    // specified hardware model
    MCPXRevision mcpxRevision = MCPXRevisionFromHardwareModel(m_settings.hw_model);
    SMCRevision smcRevision = SMCRevisionFromHardwareModel(m_settings.hw_model);
    TVEncoder tvEncoder = TVEncoderFromHardwareModel(m_settings.hw_model);

    log_debug("Initializing devices\n");

    // Create IRQs
    m_GSI = new GSI();
    m_IRQs = AllocateIRQs(m_GSI, GSI_NUM_PINS);

    // Create basic system devices
    m_i8259 = new i8259(m_cpu);
    m_i8254 = new i8254(m_i8259, m_settings.hw_sysclock_tickRate);
    m_CMOS = new CMOS();

    // TODO: make this configurable, similar to Super I/O port char drivers
    m_ataDrivers[0][0] = new hw::ata::DummyHardDriveATADeviceDriver();
    m_ataDrivers[0][1] = new hw::ata::NullATADeviceDriver();
    m_ataDrivers[1][0] = new hw::ata::NullATADeviceDriver();
    m_ataDrivers[1][1] = new hw::ata::NullATADeviceDriver();

    m_ATA = new hw::ata::ATA(m_i8259);
    m_ATA->GetChannel(hw::ata::ChanPrimary).GetDevice(0).SetDeviceDriver(m_ataDrivers[0][0]);
    m_ATA->GetChannel(hw::ata::ChanPrimary).GetDevice(1).SetDeviceDriver(m_ataDrivers[0][1]);
    m_ATA->GetChannel(hw::ata::ChanSecondary).GetDevice(0).SetDeviceDriver(m_ataDrivers[1][0]);
    m_ATA->GetChannel(hw::ata::ChanSecondary).GetDevice(1).SetDeviceDriver(m_ataDrivers[1][1]);

    if (m_settings.hw_enableSuperIO) {
        for (int i = 0; i < SUPERIO_SERIAL_PORT_COUNT; i++) {
            switch (m_settings.hw_charDrivers[i].type) {
            case CHD_Null:
                m_CharDrivers[i] = new NullCharDriver();
                break;
            case CHD_HostSerialPort:
#ifdef _WIN32
                m_CharDrivers[i] = new Win32SerialDriver(m_settings.hw_charDrivers[i].params.hostSerialPort.portNum);
#else
                m_CharDrivers[i] = new NullCharDriver(); // TODO: LinuxSerialDriver(m_settings.hw_charDrivers[i].params.hostSerialPort.portNum);
#endif
                break;
            }
            m_CharDrivers[i]->Init();
        }
        m_SuperIO = new SuperIO(m_i8259, m_CharDrivers);
        m_SuperIO->Init();
    }
    else {
        m_SuperIO = nullptr;
    }

    m_i8259->Reset();
    m_i8254->Reset();
    m_CMOS->Reset();
    m_ATA->Reset();
    if (m_settings.hw_enableSuperIO) {
        m_SuperIO->Reset();
    }

    // Create PCI bus and devices
    m_PCIBus = new PCIBus();

    m_SMC = new SMCDevice(smcRevision);
    m_EEPROM = new EEPROMDevice();
    m_HostBridge = new HostBridgeDevice();
    m_MCPXRAM = new MCPXRAMDevice(mcpxRevision);
    m_LPC = new LPCDevice(m_IRQs, m_rom, m_bios, m_biosSize, m_mcpxROM, m_settings.hw_model != DebugKit);
    m_USB1 = new USBPCIDevice(1, m_cpu);
    m_USB2 = new USBPCIDevice(9, m_cpu);
    m_NVNet = new NVNetDevice();
    m_NVAPU = new NVAPUDevice();
    m_AC97 = new AC97Device();
    m_PCIBridge = new PCIBridgeDevice();
    m_BMIDE = new BMIDEDevice(m_ram, m_ramSize);
    m_AGPBridge = new AGPBridgeDevice();
    m_NV2A = new NV2ADevice(m_ram, m_ramSize, m_i8259);

    // Configure IRQs
    m_acpiIRQs = AllocateIRQs(m_LPC, 2);
    // TODO: do we need to create an IRQ for the CPU?
    // TODO: do we need to connect the i8259 to the ISA bus?
    m_i8259IRQs = AllocateIRQs(m_i8259, ISA_NUM_IRQS);
    for (uint8_t i = 0; i < ISA_NUM_IRQS; i++) {
        m_GSI->i8259IRQs[i] = &m_i8259IRQs[i];
    }

    // Create SMBus and connect devices
    m_SMBus = new SMBus(&m_acpiIRQs[1]);

    m_SMBus->ConnectDevice(kSMBusAddress_SystemMicroController, m_SMC); // W 0x20 R 0x21
    m_SMBus->ConnectDevice(kSMBusAddress_EEPROM, m_EEPROM); // W 0xA8 R 0xA9

    // TODO: Other SMBus devices to connect
    //m_SMBus->ConnectDevice(kSMBusAddress_MCPX, m_MCPX); // W 0x10 R 0x11 -- TODO : Is MCPX an SMBus and/or PCI device?
    //m_SMBus->ConnectDevice(kSMBusAddress_TemperatureMeasurement, m_TemperatureMeasurement); // W 0x98 R 0x99
    //m_SMBus->ConnectDevice(kSMBusAddress_TVEncoder, m_TVEncoder); // W 0x88 R 0x89
    switch (tvEncoder) {
    case TVEncoder::Conexant:
        m_TVEncoder = new TVEncConexantDevice();
        m_SMBus->ConnectDevice(kSMBusAddress_TVEncoder_ID_Conexant, m_TVEncoder); // W 0x8A R 0x8B
        break;
    case TVEncoder::Focus:
        // m_TVEncoder = new TVEncFocusDevice();
        // m_SMBus->ConnectDevice(kSMBusAddress_TVEncoder_ID_Focus, m_TVEncoder); // W 0xD4 R 0xD5
        break;
    case TVEncoder::XCalibur:
        // m_TVEncoder = new TVEncXCaliburDevice();
        // m_SMBus->ConnectDevice(kSMBusAddress_TVEncoder_ID_XCalibur, m_TVEncoder); // W 0xE0 R 0xE1
        break;
    }

    // Connect devices to PCI bus
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(0, 0)), m_HostBridge);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(0, 3)), m_MCPXRAM);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 0)), m_LPC);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 1)), m_SMBus);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(2, 0)), m_USB2);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(3, 0)), m_USB1);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 0)), m_NVNet);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(5, 0)), m_NVAPU);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(6, 0)), m_AC97);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(8, 0)), m_PCIBridge);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(9, 0)), m_BMIDE);
    m_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(30, 0)), m_AGPBridge);
    m_PCIBus->ConnectDevice(PCI_DEVID(1, PCI_DEVFN(0, 0)), m_NV2A);

    // Configure PCI Bus IRQ mapper
    m_PCIBus->ConfigureIRQs(new LPCIRQMapper(m_LPC), XBOX_NUM_INT_IRQS + XBOX_NUM_PIRQS);

    // Map I/O ports and MMIO addresses
    m_i8259->MapIO(&m_ioMapper);
    m_i8254->MapIO(&m_ioMapper);
    m_CMOS->MapIO(&m_ioMapper);
    m_ATA->MapIO(&m_ioMapper);
    m_PCIBus->MapIO(&m_ioMapper);
    if (m_settings.hw_enableSuperIO) {
        m_SuperIO->MapIO(&m_ioMapper);
    }

    // TODO: Handle other SMBUS Addresses, like PIC_ADDRESS, XCALIBUR_ADDRESS
    // Resources:
    // http://pablot.com/misc/fancontroller.cpp
    // https://github.com/JayFoxRox/Chihiro-Launcher/blob/master/hook.h
    // https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
    // https://web.archive.org/web/20100617022549/http://www.xbox-linux.org/wiki/PIC

    // Write EEPROM
    // TODO: user-specified EEPROM
    m_EEPROM->SetEEPROM(kDefaultEEPROM);

    return EMUS_OK;
}

EmulatorStatus Xbox::InitDebugger() {
    // GDB Server
    if (m_settings.gdb_enable) {
        m_gdb = new GdbServer(m_cpu, "127.0.0.1", 9269);
        // TODO: handle result properly
        int result = m_gdb->Initialize();
        if (result) {
            return EMUS_INIT_DEBUGGER_FAILED;
        }

        // Allow debugging before running so client can setup breakpoints, etc
        log_debug("Starting GDB Server\n");
        m_gdb->WaitForConnection();
        m_gdb->Debug(1);
    }

    /*HardwareBreakpoints bps = { 0 };
    bps.bp[0].globalEnable = true;
    bps.bp[0].address = 0x80016756;
    bps.bp[0].length = HWBP_LENGTH_1_BYTE;
    bps.bp[0].trigger = HWBP_TRIGGER_EXECUTION;
    m_cpu->SetHardwareBreakpoints(bps);*/
    
    return EMUS_OK;
}

/*!
 * Advances the CPU emulation state.
 */
int Xbox::RunCpu()
{
#if defined(_DEBUG) && 0
    Timer t;
#endif
    int result;
    struct CpuExitInfo *exit_info;

    if (!m_should_run) {
        return -1;
    }

    while (m_should_run) {
        // Run CPU emulation
#if defined(_DEBUG) && 0
        t.Start();
#endif
        if (m_settings.cpu_singleStep) {
            result = m_cpu->Step();
        }
        else {
            result = m_cpu->Run();
        }
#if defined(_DEBUG) && 0
        t.Stop();
        log_debug("CPU Executed for %lld ms\n", t.GetMillisecondsElapsed());
#endif

#if defined(_DEBUG) && 0
        // Print CPU registers
        uint32_t eip;
        m_cpu->RegRead(REG_EIP, &eip);
        DumpCPURegisters(m_cpu);
#endif

#if defined(_DEBUG) && 0
        // Print current EIP and instruction
        static bool printEIP = false;
        uint32_t eip;
        m_cpu->RegRead(REG_EIP, &eip);
        log_debug("%08x", eip);
        {
            char mem[16];
            if (m_cpu->VMemRead(eip, 16, mem)) {
                m_cpu->MemRead(eip, 16, mem);
            }

            ZydisDecoder decoder;
            ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_ADDRESS_WIDTH_32);

            ZydisFormatter formatter;
            ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

            uint32_t offset = 0;
            ZydisDecodedInstruction instruction;

            if (ZYDIS_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, mem, sizeof(mem), eip, &instruction))) {
                char buffer[256];
                ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer));
                log_debug("    %s", buffer);
            }
        }
        log_debug("\n");
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
                DumpCPUDisassembly(m_cpu, eip, 0x40, true);
                DumpCPUDisassembly(m_cpu, eip, 0x40, false);
            }
            // Stop emulation
            Stop();
            break;
        }

        // Parse fatal error code
        uint8_t smcErrorCode = m_SMC->GetRegister(SMCRegister::ErrorCode);

        // Display fatal error code and description
        // See http://xboxdevwiki.net/Fatal_Error
        // See https://assemblergames.com/threads/xbox-error-codes-repair-reference-tips.62966/
        if (smcErrorCode != 0 && m_lastSMCErrorCode != smcErrorCode) {
            log_error("/!\\ --------------------------------- /!\\\n");
            log_fatal("/!\\    System issued a Fatal Error    /!\\\n");
            log_fatal("/!\\                                   /!\\\n");
            log_fatal("/!\\        Fatal error code %02d        /!\\\n", smcErrorCode);
            switch (smcErrorCode) {
            case  2: log_fatal("/!\\      Invalid EEPROM checksum      /!\\\n"); break;
            case  4: log_fatal("/!\\         RAM check failure         /!\\\n"); break;
            case  5: log_fatal("/!\\       Hard drive not locked       /!\\\n"); break;
            case  6: log_fatal("/!\\    Unable to unlock hard drive    /!\\\n"); break;
            case  7: log_fatal("/!\\        Hard drive timeout         /!\\\n"); break;
            case  8: log_fatal("/!\\        No hard drive found        /!\\\n"); break;
            case  9: log_fatal("/!\\  Hard drive configuration failed  /!\\\n"); break;
            case 10: log_fatal("/!\\         DVD drive timeout         /!\\\n"); break;
            case 11: log_fatal("/!\\        No DVD drive found         /!\\\n"); break;
            case 12: log_fatal("/!\\  DVD drive configuration failed   /!\\\n"); break;
            case 13: log_fatal("/!\\    Dashboard failed to launch     /!\\\n"); break;
            case 14: log_fatal("/!\\    Unspecified dashboard error    /!\\\n"); break;
            case 16: log_fatal("/!\\     Dashboard settings error      /!\\\n"); break;
            case 20: log_fatal("/!\\    Dashboard failed to launch     /!\\\n"); /* */
                /**/ log_fatal("/!\\    (DVD authentication passed)    /!\\\n"); break;
            case 21: log_fatal("/!\\         Unspecified error         /!\\\n"); break;
            default: log_fatal("/!\\              Unknown              /!\\\n"); break;
            }
            log_fatal("/!\\                                   /!\\\n");
            log_fatal("/!\\ --------------------------------- /!\\\n");

            // Stop emulation on fatal errors if configured to do so
            if (m_settings.emu_stopOnSMCFatalErrors) {
                log_fatal("Received fatal error %02d; stopping.\n", smcErrorCode);
                Stop();
                break;
            }
            m_lastSMCErrorCode = smcErrorCode;
        }

        // Print kernel bugchecks and wait for input
        if (m_settings.emu_stopOnBugChecks && LocateKernelData()) {
            uint32_t bugCheckCode[5] = { 0 };
            if (m_cpu->VMemRead(m_kExp_KiBugCheckData, 5 * sizeof(uint32_t), &bugCheckCode) == 0) {
                if (bugCheckCode[0] != 0 && m_lastBugCheckCode != bugCheckCode[0]) {
                    log_fatal("/!\\ ---------------------------- /!\\\n");
                    log_fatal("/!\\   System issued a BugCheck   /!\\\n");
                    log_fatal("/!\\                              /!\\\n");
                    log_fatal("/!\\  BugCheck code   0x%08x  /!\\\n", bugCheckCode[0]);
                    log_fatal("/!\\  Parameter 1     0x%08x  /!\\\n", bugCheckCode[1]);
                    log_fatal("/!\\  Parameter 2     0x%08x  /!\\\n", bugCheckCode[2]);
                    log_fatal("/!\\  Parameter 3     0x%08x  /!\\\n", bugCheckCode[3]);
                    log_fatal("/!\\  Parameter 4     0x%08x  /!\\\n", bugCheckCode[4]);
                    log_fatal("/!\\                              /!\\\n");
                    log_fatal("/!\\ ---------------------------- /!\\\n");
                    m_lastBugCheckCode = bugCheckCode[0];
                }
            }
        }

        // Handle reason for the CPU to exit
        exit_info = m_cpu->GetExitInfo();
        switch (exit_info->reason) {
        case CPU_EXIT_HLT:      log_info("CPU halted\n");          Stop(); break;
        case CPU_EXIT_SHUTDOWN: log_info("VM is shutting down\n"); Stop(); break;
        case CPU_EXIT_HW_BREAKPOINT:
        case CPU_EXIT_SW_BREAKPOINT:
        {
            uint32_t bpAddr;
            if (m_cpu->GetBreakpointAddress(&bpAddr) == CPUS_OP_OK) {
                if (exit_info->reason == CPU_EXIT_HW_BREAKPOINT) {
                    log_info("Hardware breakpoint hit at 0x%08x\n", bpAddr);
                }
                else {
                    log_info("Software breakpoint hit at 0x%08x\n", bpAddr);
                }
                DumpCPURegisters(m_cpu);
                DumpCPUStack(m_cpu, -0x10, 0x100);
                // TODO: allow users to handle breakpoints
                //log_warning("Press ENTER to continue\n");
                //getchar();
            }
            else {
                log_warning("Breakpoint hit, but address could not be retrieved\n");
                // TODO: allow users to handle breakpoints
                //log_warning("Press ENTER to continue\n");
                //getchar();
            }
            break;
        }
        default: break;
        }
    }

    return result;
}

void Xbox::Cleanup() {
    if (LOG_LEVEL >= LOG_LEVEL_DEBUG) {
        log_debug("CPU state at the end of execution:\n");
        uint32_t eip;
        m_cpu->RegRead(REG_EIP, &eip);
        DumpCPURegisters(m_cpu);
        if (m_settings.debug_dumpStackOnExit) {
            DumpCPUStack(m_cpu, -m_settings.debug_dumpStack_upperBound, m_settings.debug_dumpStack_lowerBound);
        }
        if (m_settings.debug_dumpDisassemblyOnExit) {
            DumpCPUDisassembly(m_cpu, eip, m_settings.debug_dumpDisassembly_length, true);
            DumpCPUDisassembly(m_cpu, eip, m_settings.debug_dumpDisassembly_length, false);
        }

#if 0
        {
            // Dump kernel to disk
            FILE *fp = NULL;
            errno_t err = fopen_s(&fp, "xboxkrnl.exe", "wb");
            if (err != 0) {
                log_debug("Could not open xboxkrnl.exe\n");
            }
            else {
                log_debug("Dumping kernel to xboxkrnl.exe... ");
                uint8_t buf[0x10000];
                uint32_t totalLen = 0;
                uint32_t len = 0;
                while (m_cpu->VMemRead(0x80010000 + totalLen, sizeof(buf), buf, &len) == CPUS_OP_OK && len > 0) {
                    totalLen += len;
                    fwrite(buf, sizeof(uint8_t), len, fp);
                }
                log_debug("%d KiB -- OK\n", totalLen / 1024);
                fclose(fp);
            }
        }
#endif

        if (m_settings.debug_dumpPageTables) {
            uint32_t cr0;
            m_cpu->RegRead(REG_CR0, &cr0);
            if (cr0 & (CR0_PG | CR0_PE)) {
                log_debug("\nPage tables:\n");
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

    if (m_settings.gdb_enable) {
        m_gdb->Shutdown();
    }
}

// CPU emulation thread function
uint32_t Xbox::EmuCpuThreadFunc(void *data) {
    Thread_SetName("[HW] CPU");
    Xbox *xbox = (Xbox *)data;
    return xbox->RunCpu();
}

bool Xbox::LocateKernelData() {
    // Return immediately if the kernel data has already been found
    if (m_kernelDataFound) {
        return true;
    }

    // Check if the kernel has been extracted and decrypted
    uint32_t eip;
    m_cpu->RegRead(REG_EIP, &eip);
    if (eip >= 0x80000000) {
        uint16_t mzMagic = 0;
        if (m_cpu->VMemRead(0x80010000, sizeof(uint16_t), &mzMagic)) return false;
        if (mzMagic != 0x5a4d) {
            return false;
        }
    }

    uint32_t peHeaderAddress = 0x00000000;
    uint32_t baseOfCode = 0x00000000;
    uint32_t exportsTableAddress = 0x00000000;

    // Find PE header position and ensure it matches the magic value
    if (m_cpu->VMemRead(0x8001003c, sizeof(uint32_t), &peHeaderAddress)) return false;
    peHeaderAddress += 0x80010000;
    uint16_t peMagic = 0;
    if (m_cpu->VMemRead(peHeaderAddress, sizeof(uint16_t), &peMagic)) return false;
    if (peMagic != 0x4550) {
        peHeaderAddress = 0x00000000;
        return false;
    }

    // Find base of code and address of functions to locate the exports table
    if (m_cpu->VMemRead(peHeaderAddress + 0x2c, sizeof(uint32_t), &baseOfCode)) return false;
    baseOfCode += 0x80010000;

    // Find exports table
    if (m_cpu->VMemRead(baseOfCode + 0x1c, sizeof(uint32_t), &exportsTableAddress)) return false;
    exportsTableAddress += 0x80010000;

    // Get addresses of relevant exports
#define GET_EXPORT(name, num) do { if (m_cpu->VMemRead(exportsTableAddress + ((num - 1) * sizeof(uint32_t)), sizeof(uint32_t), &m_kExp_##name)) { return false; } m_kExp_##name += 0x80010000; } while (0)
    GET_EXPORT(KiBugCheckData, 162);
    GET_EXPORT(XboxKrnlVersion, 324);
#undef GET_EXPORT

    uint32_t pKernelPEHeaderPos;
    uint32_t pKernelBaseOfCode;
    uint32_t pKernelExportsTableAddress;
    uint32_t pKiBugCheckData;
    uint32_t pXboxKrnlVersion;
    m_cpu->VirtualToPhysical(peHeaderAddress, &pKernelPEHeaderPos);
    m_cpu->VirtualToPhysical(baseOfCode, &pKernelBaseOfCode);
    m_cpu->VirtualToPhysical(exportsTableAddress, &pKernelExportsTableAddress);
    m_cpu->VirtualToPhysical(m_kExp_KiBugCheckData, &pKiBugCheckData);
    m_cpu->VirtualToPhysical(m_kExp_XboxKrnlVersion, &pXboxKrnlVersion);
    log_info("Microsoft Xbox Kernel detected\n");
    log_info("  PE header           0x%08x  ->  0x%p\n", peHeaderAddress, m_ram + pKernelPEHeaderPos);
    log_info("  Base of code        0x%08x  ->  0x%p\n", baseOfCode, m_ram + pKernelBaseOfCode);
    log_info("  Exports table       0x%08x  ->  0x%p\n", exportsTableAddress, m_ram + pKernelExportsTableAddress);
    log_info("    KiBugCheckData    0x%08x  ->  0x%p\n", m_kExp_KiBugCheckData, m_ram + pKiBugCheckData);
    log_info("    XboxKrnlVersion   0x%08x  ->  0x%p\n", m_kExp_XboxKrnlVersion, m_ram + pXboxKrnlVersion);
    m_kernelDataFound = true;

    m_cpu->VMemRead(m_kExp_XboxKrnlVersion, sizeof(XboxKernelVersion), &m_kernelVersion);
    log_info("Xbox kernel version: %d.%d.%d.%d\n", m_kernelVersion.major, m_kernelVersion.minor, m_kernelVersion.build, m_kernelVersion.rev);

    return true;
}
}
