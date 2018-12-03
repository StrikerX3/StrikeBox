#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include "lib/cxxopts.hpp"

#include "openxbox/core.h"
#include "openxbox/settings.h"
#include "openxbox/thread.h"

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
    openxbox::Thread_SetName("[Core] Main Thread");

	using namespace openxbox;

	auto info = GetOpenXBOXInfo();
	printf("OpenXBOX v%s\n", info->version);
	printf("------------------\n");

	cxxopts::Options options(basename((char*)argv[0]), "Open source XBOX Emulator\n");
	options.custom_help("-c mcpx_path -b bios_path -m model");
	options.add_options()
		("c, mcpx", "MCPX path", cxxopts::value<std::string>(), "mcpx_path")
		("b, bios", "BIOS path", cxxopts::value<std::string>(), "bios_path")
		("m, model", "XBOX Model (retail | debug)", cxxopts::value<std::string>(), "xbox_model")
		("h, help", "Shows this message");

	auto args = options.parse(argc, argv);

	/*
		Argument Check
	*/
	if (!(args.count("mcpx") && args.count("bios") && args.count("model")))
	{
		std::cout << options.help();
		return 1;
	}

	// Parse arguments
	const char *mcpx_path = args["mcpx"].as<std::string>().c_str();
	const char *bios_path = args["bios"].as<std::string>().c_str();
	const char *model = args["model"].as<std::string>().c_str();
	bool is_debug;

	if (strcmp(model, "debug") == 0) {
		is_debug = true;
		printf("Emulating debug console.\n");
	}
	else if (strcmp(model, "retail") == 0) {
		is_debug = false;
		printf("Emulating retail console.\n");
	}
	else {
		printf("Invalid model specified.\n");
		std::cout << options.help();
		return 1;
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

    OpenXBOXSettings *settings = xbox->GetSettings();
    settings->cpu_singleStep = false;
    settings->ram_expanded = true;
    settings->emu_stopOnSMCFatalErrors = true;
    settings->emu_stopOnBugChecks = true;
    settings->debug_dumpPageTables = false;
    settings->debug_dumpDisassemblyOnExit = true;
    settings->debug_dumpDisassembly_length = 10;
    settings->debug_dumpStackOnExit = true;
    settings->debug_dumpStack_upperBound = 0x10;
    settings->debug_dumpStack_lowerBound = 0x20;
    settings->gdb_enable = false;
    settings->hw_model = is_debug ? DebugKit : Revision1_0;
    settings->hw_sysclock_tickRate = 1000.0f;
    settings->hw_enableSuperIO = true;
    settings->hw_charDrivers[0].type = CHD_HostSerialPort;
    settings->hw_charDrivers[0].params.hostSerialPort.portNum = 5;
    //settings->hw_charDrivers[0].type = CHD_Null;
    settings->hw_charDrivers[1].type = CHD_Null;
    settings->rom_mcpx = mcpx_path;
    settings->rom_bios = bios_path;

    EmulatorStatus status = xbox->Run();
    if (status == EMUS_OK) {
        log_info("Emulator exited successfully\n");
    } else {
        log_fatal("Emulator exited with error: ");
        switch (status) {
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
        case EMUS_INIT_DEBUGGER_FAILED: log_fatal("Debugger initialization failed"); break;
        default: log_fatal("Unspecified error\n"); break;
        }
    }

    delete xbox;
    return status;
}
