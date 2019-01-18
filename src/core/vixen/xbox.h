#pragma once

#include <assert.h>
#ifndef _WIN32
    #include <libgen.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "vixen/cpu.h"
#include "vixen/dev.h"
#include "vixen/emulator.h"
#include "vixen/gdbserver.h"
#include "vixen/log.h"
#include "vixen/mem.h"
#include "vixen/util.h"
#include "vixen/thread.h"
#include "vixen/settings.h"
#include "vixen/status.h"

#include "vixen/hw/basic/irq.h"
#include "vixen/hw/basic/gsi.h"
#include "vixen/hw/basic/i8254.h"
#include "vixen/hw/basic/i8259.h"
#include "vixen/hw/basic/superio.h"
#include "vixen/hw/basic/cmos.h"

#include "vixen/hw/ata/ata.h"

#include "vixen/hw/bus/smbus.h"
#include "vixen/hw/bus/pcibus.h"

#include "vixen/hw/sm/smc.h"
#include "vixen/hw/sm/eeprom.h"
#include "vixen/hw/sm/tvenc.h"
#include "vixen/hw/sm/adm1032.h"

#include "vixen/hw/pci/hostbridge.h"
#include "vixen/hw/pci/mcpx_ram.h"
#include "vixen/hw/pci/lpc.h"
#include "vixen/hw/pci/agpbridge.h"
#include "vixen/hw/pci/pcibridge.h"
#include "vixen/hw/pci/usb_pci.h"
#include "vixen/hw/pci/nvnet.h"
#include "vixen/hw/pci/nvapu.h"
#include "vixen/hw/pci/ac97.h"
#include "vixen/hw/pci/bmide.h"
#include "vixen/hw/pci/nv2a.h"

#include "vixen/cpu_module.h"

namespace vixen {

/*!
 * Xbox kernel version data structure exported by Microsoft kernels.
 */
struct XboxKernelVersion {
    uint16_t major;
    uint16_t minor;
    uint16_t build;
    uint16_t rev;
};

/*!
 * Top-level Xbox machine class
 *
 * This class is the top-level class, will perform initialization and high-level
 * management of the overall emulation flow.
 */
class Xbox : Emulator {
public:
    Xbox(vixen::modules::cpu::ICPUModule *cpuModule);
    virtual ~Xbox();

    viXenSettings& GetSettings() { return m_settings; }
    void CopySettings(viXenSettings *settings);

    EmulatorStatus Run();
    void Stop();

protected:
    // ----- Initialization and cleanup ---------------------------------------
    EmulatorStatus Initialize();
    EmulatorStatus InitFixupSettings();
    EmulatorStatus InitMemory();
    EmulatorStatus InitRAM();
    EmulatorStatus InitROM();
    EmulatorStatus InitCPU();
    EmulatorStatus InitHardware();
    EmulatorStatus InitDebugger();

    void Cleanup();

    // ----- Thread functions -------------------------------------------------
    int RunCpu();

    // ----- Friends ----------------------------------------------------------
    static uint32_t EmuCpuThreadFunc(void *data);

    // ----- Modules ----------------------------------------------------------
    vixen::modules::cpu::ICPUModule *m_cpuModule;

    // ----- Hardware ---------------------------------------------------------
    Cpu              *m_cpu = nullptr;
    uint32_t          m_ramSize = 0;
    uint8_t          *m_ram = nullptr;
    uint8_t          *m_rom = nullptr;
    uint8_t          *m_bios = nullptr;
    uint32_t          m_biosSize = 0;
    uint8_t          *m_mcpxROM = nullptr;
    MemoryRegion     *m_memRegion = nullptr;
    IOMapper          m_ioMapper;

    GSI              *m_GSI = nullptr;
    IRQ              *m_IRQs = nullptr;
    IRQ              *m_acpiIRQs = nullptr;
    IRQ              *m_i8259IRQs = nullptr;

    i8254            *m_i8254 = nullptr;
    i8259            *m_i8259 = nullptr;
    CMOS             *m_CMOS = nullptr;
    hw::ata::ATA     *m_ATA = nullptr;
    hw::ata::IATADeviceDriver *m_ataDrivers[2][2] = { { nullptr } };
    CharDriver       *m_CharDrivers[SUPERIO_SERIAL_PORT_COUNT] = { nullptr };
    SuperIO          *m_SuperIO = nullptr;

    SMBus            *m_SMBus = nullptr;
    SMCDevice        *m_SMC = nullptr;
    EEPROMDevice     *m_EEPROM = nullptr;
    TVEncoderDevice  *m_TVEncoder = nullptr;
    ADM1032Device    *m_ADM1032 = nullptr;

    PCIBus           *m_PCIBus = nullptr;
    HostBridgeDevice *m_HostBridge = nullptr;
    MCPXRAMDevice    *m_MCPXRAM = nullptr;
    LPCDevice        *m_LPC = nullptr;
    USBPCIDevice     *m_USB1 = nullptr;
    USBPCIDevice     *m_USB2 = nullptr;
    NVNetDevice      *m_NVNet = nullptr;
    NVAPUDevice      *m_NVAPU = nullptr;
    AC97Device       *m_AC97 = nullptr;
    PCIBridgeDevice  *m_PCIBridge = nullptr;
    hw::bmide::BMIDEDevice *m_BMIDE = nullptr;
    AGPBridgeDevice  *m_AGPBridge = nullptr;
    NV2ADevice       *m_NV2A = nullptr;

    // ----- Configuration ----------------------------------------------------
    viXenSettings     m_settings;

    // ----- State ------------------------------------------------------------
    bool     m_should_run;

    uint8_t  m_lastSMCErrorCode = 0;
    uint32_t m_lastBugCheckCode = 0x00000000;
    
    XboxKernelVersion m_kernelVersion = { 0 };

    bool     m_kernelDataFound = false;
    uint32_t m_kExp_KiBugCheckData = 0x00000000;
    uint32_t m_kExp_XboxKrnlVersion = 0x00000000;

    bool LocateKernelData();

    // ----- Debugger ---------------------------------------------------------
    GdbServer *m_gdb;
};

}
