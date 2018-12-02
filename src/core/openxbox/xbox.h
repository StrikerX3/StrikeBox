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

#include "openxbox/cpu.h"
#include "openxbox/dev.h"
#include "openxbox/emulator.h"
#include "openxbox/gdbserver.h"
#include "openxbox/log.h"
#include "openxbox/mem.h"
#include "openxbox/util.h"
#include "openxbox/thread.h"
#include "openxbox/settings.h"

#include "openxbox/hw/basic/irq.h"
#include "openxbox/hw/basic/gsi.h"
#include "openxbox/hw/basic/i8254.h"
#include "openxbox/hw/basic/i8259.h"
#include "openxbox/hw/basic/superio.h"
#include "openxbox/hw/basic/cmos.h"

#include "openxbox/hw/bus/smbus.h"
#include "openxbox/hw/bus/pcibus.h"

#include "openxbox/hw/sm/smc.h"
#include "openxbox/hw/sm/eeprom.h"
#include "openxbox/hw/sm/tvenc.h"

#include "openxbox/hw/pci/hostbridge.h"
#include "openxbox/hw/pci/mcpx_ram.h"
#include "openxbox/hw/pci/lpc.h"
#include "openxbox/hw/pci/agpbridge.h"
#include "openxbox/hw/pci/pcibridge.h"
#include "openxbox/hw/pci/usb_pci.h"
#include "openxbox/hw/pci/nvnet.h"
#include "openxbox/hw/pci/nvapu.h"
#include "openxbox/hw/pci/ac97.h"
#include "openxbox/hw/pci/ide.h"
#include "openxbox/hw/pci/nv2a.h"

#include "openxbox/cpu_module.h"

namespace openxbox {

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
    Xbox(IOpenXBOXCPUModule *cpuModule);
    virtual ~Xbox();

    OpenXBOXSettings *GetSettings() { return &m_settings; }
    void CopySettings(OpenXBOXSettings *settings);

    int Run();
    void Stop();

protected:
    // ----- Initialization and cleanup ---------------------------------------
    int Initialize();
    void InitFixupSettings();
    int InitMemory();
    int InitRAM();
    int InitROM();
    int InitCPU();
    int InitHardware();
    int InitDebugger();

    void Cleanup();

    // ----- Thread functions -------------------------------------------------
    int RunCpu();

    // ----- Friends ----------------------------------------------------------
    static uint32_t EmuCpuThreadFunc(void *data);

    // ----- Modules ----------------------------------------------------------
    IOpenXBOXCPUModule *m_cpuModule;

    // ----- Hardware ---------------------------------------------------------
    Cpu              *m_cpu;
    uint32_t          m_ramSize;
    uint8_t          *m_ram;
    uint8_t          *m_rom;
    uint8_t          *m_bios;
    uint32_t          m_biosSize;
    uint8_t          *m_mcpxROM;
    MemoryRegion     *m_memRegion;
    IOMapper          m_ioMapper;
    
    GSI              *m_GSI;
    IRQ              *m_IRQs;
    IRQ              *m_acpiIRQs;
    IRQ              *m_i8259IRQs;

    i8254            *m_i8254;
    i8259            *m_i8259;
    CMOS             *m_CMOS;
    CharDriver       *m_CharDrivers[SUPERIO_SERIAL_PORT_COUNT];
    SuperIO          *m_SuperIO;

    SMBus            *m_SMBus;
    SMCDevice        *m_SMC;
    EEPROMDevice     *m_EEPROM;
    TVEncoderDevice  *m_TVEncoder;

    PCIBus           *m_PCIBus;
    HostBridgeDevice *m_HostBridge;
    MCPXRAMDevice    *m_MCPXRAM;
    LPCDevice        *m_LPC;
    USBPCIDevice     *m_USB1;
    USBPCIDevice     *m_USB2;
    NVNetDevice      *m_NVNet;
    NVAPUDevice      *m_NVAPU;
    AC97Device       *m_AC97;
    PCIBridgeDevice  *m_PCIBridge;
    IDEDevice        *m_IDE;
    AGPBridgeDevice  *m_AGPBridge;
    NV2ADevice       *m_NV2A;

    // ----- Configuration ----------------------------------------------------
    OpenXBOXSettings  m_settings;

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
