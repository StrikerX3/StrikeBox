#pragma once

#include "openxbox/hw/defs.h"

namespace openxbox {

enum CharDriverType {
    CHD_Null,
    CHD_HostSerialPort,
};

struct OpenXBOXSettings {
    // false: the CPU emulator will execute until interrupted
    // true: the CPU emulator will execute one instruction at a time
    bool cpu_singleStep = false;

    // true: the emulator will stop on a fatal error
    bool emu_stopOnSMCFatalErrors = false;

    // true: the emulator will stop on a kernel bug check
    // (only applies to original or modified Microsoft kernels)
    bool emu_stopOnBugChecks = false;

    // true: enables the GDB server, allowing the guest to be debugged
    bool gdb_enable = false;

    // true: XBE sections will be dumped to individual files
    bool debug_dumpXBESectionContents = false;

    // true: dump page tables on exit
    bool debug_dumpPageTables = false;

    // true: dump current stack on exit
    bool debug_dumpStackOnExit = false;

    // The upper bound of the stack to be dumped, expressed as an offset from the current stack pointer
    uint32_t debug_dumpStack_upperBound = 0x10;

    // The lower bound of the stack to be dumped, expressed as an offset from the current stack pointer
    uint32_t debug_dumpStack_lowerBound = 0x20;

    // true: dump disassembly of instructions starting at current EIP on exit
    bool debug_dumpDisassemblyOnExit = false;

    // The number of instructions to disassemble
    uint32_t debug_dumpDisassembly_length = 15;

    // The Xbox hardware model to use
    HardwareModel hw_model = DebugKit;

    // The system clock tick rate
    float hw_sysclock_tickRate = 1000.0f;

    // Enable Super I/O hardware on retail systems
    // Always enabled on DebugKit models
    bool hw_enableSuperIO = true;

    // Character I/O drivers for each of the two emulated serial ports
    struct {
        CharDriverType type;
        union {
            struct {
                uint8_t portNum;
            } hostSerialPort;
        } params;
    } hw_charDrivers[2];

    // Path to MCPX ROM file
    const char *rom_mcpx;

    // Path to BIOS ROM file
    const char *rom_bios;

    // TODO: path to game XISO or XBE from extracted folder
};

}
