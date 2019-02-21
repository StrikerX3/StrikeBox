#pragma once

namespace vixen {

enum EmulatorStatus {
    EMUS_OK = 0,                         // Operation completed successfully

    EMUS_INIT_INVALID_REVISION,          // An invalid hardware revision was specified

    EMUS_INIT_VM_INIT_FAILED,            // Could not initialize virtual machine

    EMUS_INIT_ALLOC_RAM_FAILED,          // Could not allocate memory for the guest RAM
    EMUS_INIT_ALLOC_RAM_RGN_FAILED,      // Could not allocate memory for the RAM region

    EMUS_INIT_ALLOC_ROM_FAILED,          // Could not allocate memory for the guest ROM
    EMUS_INIT_ALLOC_ROM_RGN_FAILED,      // Could not allocate memory for the ROM region
    EMUS_INIT_MCPX_ROM_NOT_FOUND,        // MCPX ROM file not found
    EMUS_INIT_MCPX_ROM_INVALID_SIZE,     // MCPX ROM provided has incorrect size (must be 512 bytes)
    EMUS_INIT_BIOS_ROM_NOT_FOUND,        // BIOS ROM file not found
    EMUS_INIT_BIOS_ROM_INVALID_SIZE,     // BIOS ROM provided has incorrect size (must be 256 KiB or 1 MiB)

    EMUS_INIT_INVALID_HARD_DRIVE_TYPE,   // An invalid hard drive type was specified
    EMUS_INIT_HARD_DRIVE_INIT_FAILED,    // Virtual hard drive initialization failed
    EMUS_INIT_INVALID_DVD_DRIVE_TYPE,    // An invalid DVD drive type was specified
    EMUS_INIT_DVD_DRIVE_INIT_FAILED,     // Virtual DVD drive initialization failed

    EMUS_INIT_DEBUGGER_FAILED,           // Debugger initialization failed
};

}
