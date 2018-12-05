#pragma once

namespace vixen {

enum EmulatorStatus {
    EMUS_OK = 0,                      // Operation completed successfully

    EMUS_INIT_ALLOC_MEM_RGN_FAILED,   // Could not allocate memory for the global memory region

    EMUS_INIT_ALLOC_RAM_FAILED,       // Could not allocate memory for the guest RAM
    EMUS_INIT_ALLOC_RAM_RGN_FAILED,   // Could not allocate memory for the RAM region

    EMUS_INIT_ALLOC_ROM_FAILED,       // Could not allocate memory for the guest ROM
    EMUS_INIT_ALLOC_ROM_RGN_FAILED,   // Could not allocate memory for the ROM region
    EMUS_INIT_MCPX_ROM_NOT_FOUND,     // MCPX ROM file not found
    EMUS_INIT_MCPX_ROM_INVALID_SIZE,  // MCPX ROM provided has incorrect size (must be 512 bytes)
    EMUS_INIT_BIOS_ROM_NOT_FOUND,     // BIOS ROM file not found
    EMUS_INIT_BIOS_ROM_INVALID_SIZE,  // BIOS ROM provided has incorrect size (must be 256 KiB or 1 MiB)

    EMUS_INIT_NO_CPU_MODULE,          // No CPU module specified
    EMUS_INIT_CPU_CREATE_FAILED,      // CPU instantiation failed
    EMUS_INIT_CPU_INIT_FAILED,        // CPU initialization failed
    EMUS_INIT_CPU_MEM_MAP_FAILED,     // Memory mapping failed

    EMUS_INIT_DEBUGGER_FAILED,        // Debugger initialization failed
};

enum CPUInitStatus {
    CPUS_INIT_OK = 0,                 // CPU initialized successfully

    CPUS_INIT_PLATFORM_INIT_FAILED,   // Failed to initialize emulation or virtualization platform
    CPUS_INIT_CREATE_VM_FAILED,       // Failed to create virtual machine
    CPUS_INIT_CREATE_CPU_FAILED,      // Failed to create CPU
};

enum CPUStatus {
    CPUS_OK = 0,                      // CPU executed successfully

    CPUS_FAILED,                      // CPU emulation failed due to an unspecified error
    CPUS_UNSUPPORTED,                 // CPU emulator does not support the operation
};

enum CPUMemMapStatus {
    CPUS_MMAP_OK = 0,                 // Memory mapped successfully

    CPUS_MMAP_CPU_UNINITIALIZED,      // The virtual CPU is not initialized
    CPUS_MMAP_INVALID_TYPE,           // Invalid memory region type
    CPUS_MMAP_MEMORY_ADDR_MISALIGNED, // The provided memory region is misaligned
    CPUS_MMAP_MEMORY_SIZE_MISALIGNED, // The provided memory region size is misaligned
    CPUS_MMAP_MAPPING_FAILED,         // Failed to map the memory region

    CPUS_MMAP_UNHANDLED_ERROR,        // An unhandled error occurred
};

enum CPUOperationStatus {
    CPUS_OP_OK = 0,                   // Operation completed succesfully

    CPUS_OP_FAILED,                   // The operation failed
    CPUS_OP_INVALID_ADDRESS,          // An invalid memory address was specified
    CPUS_OP_INVALID_SELECTOR,         // An invalid selector was specified
    CPUS_OP_INVALID_REGISTER,         // An invalid register was specified
    CPUS_OP_BREAKPOINT_NEVER_HIT,     // A breakpoint was never hit
    
    CPUS_OP_UNSUPPORTED,              // The operation is not supported
};

}
