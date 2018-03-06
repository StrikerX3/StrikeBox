#pragma once

namespace openxbox {

struct OpenXBOXSettings {
    // false: the CPU emulator will execute until interrupted
    // true: the CPU emulator will execute one instruction at a time
    bool cpu_singleStep = false;

    // true: enables the GDB server, allowing the guest to be debugged
    bool gdb_enable = false;

    // true: XBE sections will be dumped to individual files
    bool debug_dumpXBESectionContents = false;

    // true: dump memory address mappings (page tables) after execution
    bool debug_dumpMemoryMapping = false;

    // The system clock tick rate
    float hw_sysclock_tickRate = 1000.0f;

    // Path to MCPX ROM file
    const char *rom_mcpx;

    // Path to BIOS ROM file
    const char *rom_bios;

    // TODO: path to game XISO or XBE from extracted folder
};

}
