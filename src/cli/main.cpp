#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include "lib/cxxopts.hpp"

#include "vixen/core.h"
#include "vixen/settings.h"
#include "vixen/thread.h"

#ifdef _WIN32
char *basename(char *path)
{
	char name[MAX_PATH];
	char ext[MAX_PATH];
	if (_splitpath_s(path, NULL, 0, NULL, 0, name, MAX_PATH, ext, MAX_PATH)) {
		return NULL;
	}
	size_t len = strlen(name) + strlen(ext) + 1;
	char *out = (char *)malloc(len);
	strcpy(out, name);
	strcat(out, ext);
	return out;
}
#endif

/*!
 * Program entry point
 */
int main(int argc, const char *argv[]) {
    using namespace vixen;

    Thread_SetName("[Core] Main Thread");

    auto info = GetViXenInfo();
    printf("viXen v%s\n", info->version);
    printf("------------------\n");

    cxxopts::Options options(basename((char*)argv[0]), "viXen - 6th generation (Original) XBOX Emulator\n");
    options.custom_help("-m mcpx_path -b bios_path -r xbox_rev [-hd vhd_image_path]");
    options.add_options()
        ("m, mcpx", "Path to MCPX ROM", cxxopts::value<std::string>(), "mcpx_path")
        ("b, bios", "Path to BIOS ROM", cxxopts::value<std::string>(), "bios_path")
        ("d, hard-disk-image", "Path to virtual hard disk drive image", cxxopts::value<std::string>(), "vhd_image_path")
        ("r, revision", "XBOX revision (retail | debug)", cxxopts::value<std::string>(), "xbox_rev")
        ("h, help", "Shows this message");

    auto args = options.parse(argc, argv);

    // Argument Check
    if (!(args.count("mcpx") && args.count("bios") && args.count("revision"))) {
        std::cout << options.help();
        return 1;
    }

    // Parse arguments
    const char *mcpx_path = args["mcpx"].as<std::string>().c_str();
    const char *bios_path = args["bios"].as<std::string>().c_str();
    const char *revision = args["revision"].as<std::string>().c_str();
    const char *vhd_path;
    if (args.count("hard-disk-image") == 0) {
        vhd_path = "";
    }
    else {
        vhd_path = args["hard-disk-image"].as<std::string>().c_str();
    }

    // Locate and instantiate modules
    ModuleRepository moduleRepo;
    moduleRepo.Enumerate();

    // Search for CPU modules
    auto cpuModules = moduleRepo.GetCPUModules();
    if (cpuModules.size() == 0) {
        log_fatal("No CPU modules found\n");
        return -1;
    }

    // Load first CPU module found
    auto cpuModuleInfo = cpuModules[0];

    // Instantiate CPU module
    log_info("Loading CPU module: %s %s... ", cpuModuleInfo.moduleName.c_str(), cpuModuleInfo.moduleVersion.c_str());
    CPUModuleInstance cpuModuleInstance;
    auto cpuModuleResult = LoadCPUModule(cpuModuleInfo.libraryPath, &cpuModuleInstance);
    if (cpuModuleResult != kModuleLoadSuccess) {
        log_fatal("instantiation failed; result code 0x%x\n", cpuModuleResult);
        return -1;
    }
    log_info("success\n");

    Xbox *xbox = new Xbox(cpuModuleInstance.cpuModule);

    viXenSettings *settings = xbox->GetSettings();
    settings->cpu_singleStep = false;
    settings->ram_expanded = true;
    settings->emu_stopOnSMCFatalErrors = true;
    settings->emu_stopOnBugChecks = true;
    settings->debug_dumpPageTables = false;
    settings->debug_dumpDisassemblyOnExit = false;
    settings->debug_dumpDisassembly_length = 10;
    settings->debug_dumpStackOnExit = false;
    settings->debug_dumpStack_upperBound = 0x10;
    settings->debug_dumpStack_lowerBound = 0x20;
    settings->gdb_enable = false;
    settings->hw_sysclock_tickRate = 1000.0f;
    settings->hw_enableSuperIO = true;
    settings->hw_charDrivers[0].type = CHD_HostSerialPort;
    settings->hw_charDrivers[0].params.hostSerialPort.portNum = 5;
    //settings->hw_charDrivers[0].type = CHD_Null;
    settings->hw_charDrivers[1].type = CHD_Null;
    settings->rom_mcpx = mcpx_path;
    settings->rom_bios = bios_path;

    if (strcmp(revision, "debug") == 0) {
        settings->hw_revision = DebugKit;
    }
    else if (strcmp(revision, "retail") == 0) {
        settings->hw_revision = Revision1_0;
    }
    else {
        printf("Invalid revision specified.\n");
        std::cout << options.help();
        return 1;
    }

    if (strlen(vhd_path) == 0) {
        settings->vhd_type = VHD_Dummy;
    }
    else {
        settings->vhd_type = VHD_Image;
        settings->vhd_parameters.image.path = vhd_path;
        settings->vhd_parameters.image.preserveImage = true;
    }

    EmulatorStatus status = xbox->Run();
    if (status == EMUS_OK) {
        log_info("Emulator exited successfully\n");
    }
    else {
        log_fatal("Emulator exited with error: ");
        switch (status) {
        case EMUS_INIT_INVALID_REVISION: log_fatal("Invalid revision specified\n"); break;
        case EMUS_INIT_ALLOC_MEM_RGN_FAILED: log_fatal("Could not allocate memory for the global memory region\n"); break;
        case EMUS_INIT_ALLOC_RAM_FAILED: log_fatal("Could not allocate memory for the guest RAM\n"); break;
        case EMUS_INIT_ALLOC_RAM_RGN_FAILED: log_fatal("Could not allocate memory for the RAM region"); break;
        case EMUS_INIT_ALLOC_ROM_FAILED: log_fatal("Could not allocate memory for the guest ROM"); break;
        case EMUS_INIT_ALLOC_ROM_RGN_FAILED: log_fatal("Could not allocate memory for the ROM region"); break;
        case EMUS_INIT_MCPX_ROM_NOT_FOUND: log_fatal("MCPX ROM file not found"); break;
        case EMUS_INIT_MCPX_ROM_INVALID_SIZE: log_fatal("MCPX ROM provided has incorrect size (must be 512 bytes)"); break;
        case EMUS_INIT_BIOS_ROM_NOT_FOUND: log_fatal("BIOS ROM file not found"); break;
        case EMUS_INIT_BIOS_ROM_INVALID_SIZE: log_fatal("BIOS ROM provided has incorrect size (must be 256 KiB or 1 MiB)"); break;
        case EMUS_INIT_NO_CPU_MODULE: log_fatal("No CPU module specified"); break;
        case EMUS_INIT_CPU_CREATE_FAILED: log_fatal("CPU instantiation failed"); break;
        case EMUS_INIT_CPU_INIT_FAILED: log_fatal("CPU initialization failed"); break;
        case EMUS_INIT_CPU_MEM_MAP_FAILED: log_fatal("Memory mapping failed"); break;
        case EMUS_INIT_INVALID_HARD_DRIVE_TYPE: log_fatal("Invalid virtual hard drive type specified"); break;
        case EMUS_INIT_HARD_DRIVE_INIT_FAILED: log_fatal("Failed to initialize virtual hard drive"); break;
        case EMUS_INIT_INVALID_DVD_DRIVE_TYPE: log_fatal("Invalid virtual DVD drive type specified"); break;
        case EMUS_INIT_DVD_DRIVE_INIT_FAILED: log_fatal("Failed to initialize virtual DVD drive"); break;
        case EMUS_INIT_DEBUGGER_FAILED: log_fatal("Debugger initialization failed"); break;
        default: log_fatal("Unspecified error\n"); break;
        }
    }

    delete xbox;
    return status;
}
