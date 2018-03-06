#pragma once

namespace openxbox {

struct OpenXBOXSettings {
    // false: the CPU emulator will execute until interrupted
    // true: the CPU emulator will execute one instruction at a time
    bool cpu_singleStep;

    // true: enables the GDB server, allowing the guest to be debugged
    bool gdb_enable;

    // true: XBE sections will be dumped to individual files
    bool debug_dumpXBESectionContents;

    // true: dump memory address mappings (page tables) after execution
    bool debug_dumpMemoryMapping;

    // TODO: path to MCPX ROM
    // TODO: path to BIOS ROM
    // TODO: path to game XISO or extracted contents
};

}
