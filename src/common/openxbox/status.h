#pragma once

namespace openxbox {

enum XboxStatus {
    XBS_OK = 0,                      // Operation completed successfully

    XBS_INIT_ALLOC_MEM_RGN_FAILED,   // Could not allocate memory for the global memory region

    XBS_INIT_ALLOC_RAM_FAILED,       // Could not allocate memory for the guest RAM
    XBS_INIT_ALLOC_RAM_RGN_FAILED,   // Could not allocate memory for the RAM region

    XBS_INIT_ALLOC_ROM_FAILED,       // Could not allocate memory for the guest ROM
    XBS_INIT_ALLOC_ROM_RGN_FAILED,   // Could not allocate memory for the ROM region
    XBS_INIT_MCPX_ROM_NOT_FOUND,     // MCPX ROM file not found
    XBS_INIT_MCPX_ROM_INVALID_SIZE,  // MCPX ROM provided has incorrect size (must be 512 bytes)
    XBS_INIT_BIOS_ROM_NOT_FOUND,     // BIOS ROM file not found
    XBS_INIT_BIOS_ROM_INVALID_SIZE,  // BIOS ROM provided has incorrect size (must be 256 KiB or 1 MiB)

    XBS_INIT_NO_CPU_MODULE,          // No CPU module specified
    XBS_INIT_CPU_CREATE_FAILED,      // CPU instantiation failed
    XBS_INIT_CPU_INIT_FAILED,        // CPU initialization failed

    XBS_INIT_DEBUGGER_FAILED,        // Debugger initialization failed
};

}
