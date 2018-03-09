#pragma once

#include "openxbox/hw/defs.h"

namespace openxbox {

struct OpenXBOXSettings {
    // false: the CPU emulator will execute until interrupted
    // true: the CPU emulator will execute one instruction at a time
    bool cpu_singleStep = false;

    // true: enables the GDB server, allowing the guest to be debugged
    bool gdb_enable = false;

    // true: XBE sections will be dumped to individual files
    bool debug_dumpXBESectionContents = false;

    // true: dump page tables after execution
    bool debug_dumpPageTables = false;

    // The Xbox hardware model to use
    HardwareModel hw_model = DebugKit;

    // The system clock tick rate
    float hw_sysclock_tickRate = 1000.0f;

    // Path to MCPX ROM file
    const char *rom_mcpx;

    // Path to BIOS ROM file
    const char *rom_bios;

    // TODO: path to game XISO or XBE from extracted folder
};

}
