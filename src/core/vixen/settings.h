#pragma once

#include "vixen/hw/defs.h"

namespace vixen {

enum CharDriverType {
    CHD_Null,
    CHD_HostSerialPort,
};

enum VirtualHardDiskDriveType {
    VHD_Null,    // No hard disk
    VHD_Dummy,   // Dummy 10 GiB blank hard disk
    VHD_Image,   // Use an image file for the hard disk
    // TODO: VHD_HostDirectory   // Virtual disk mapped to a directory on the host
};

enum VirtualDVDDriveType {
    VDVD_Null,    // No DVD drive
    VDVD_Dummy,   // Dummy DVD drive with no media
    VDVD_Image,   // Virtual DVD with the specified DVD image
    // TODO: VDVD_HostDevice      // Direct access to a removable media drive on the host
    // TODO: VDVD_HostDirectory   // Virtual DVD drive mapped to a directory on the host
};

struct viXenSettings {
    // false: the CPU emulator will execute until interrupted
    // true: the CPU emulator will execute one instruction at a time
    bool cpu_singleStep = false;

    // false: use standard 64 MiB RAM
    // true: expand RAM to 128 MiB
    bool ram_expanded = false;

    // true: the emulator will stop on a fatal error
    bool emu_stopOnSMCFatalErrors = false;

    // true: the emulator will stop on a kernel bug check
    // (only applies to original or modified Microsoft kernels)
    bool emu_stopOnBugChecks = false;

    // true: enables the GDB server, allowing the guest to be debugged
    bool gdb_enable = false;

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

    // The Xbox hardware revision to use
    HardwareModel hw_revision = DebugKit;

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

    // Virtual hard disk drive parameters
    VirtualHardDiskDriveType vhd_type = VHD_Null;
    union {
        // VHD_Image
        struct {
            const char *path;     // Path to virtual hard disk image
            bool preserveImage;   // If true, writes will be done in a temporary file; if false, writes are done directly to the image file
        } image;
    } vhd_parameters;

    // Virtual DVD drive parameters
    VirtualDVDDriveType vdvd_type = VDVD_Null;
    union {
        // VDVD_Image: Path to DVD disk image
        struct {
            const char *path;     // Path to DVD image
            bool preserveImage;   // If true, writes will be done in a temporary file; if false, writes are done directly to the image file
        } image;
    } vdvd_parameters;
};

}
