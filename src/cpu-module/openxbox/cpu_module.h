#pragma once

#include "openxbox/module_common.h"
#include "openxbox/cpu.h"

namespace openxbox {

// The CPU module API version
// Increment this if there are any ABI breaking changes
#define OPENXBOX_CPU_MODULE_API_VERSION 1


// Base interface for OpenXBOX CPU modules
class OPENXBOX_MODULE_API IOpenXBOXCPUModule {
public:
	virtual Cpu *GetCPU() = 0;
	virtual void FreeCPU(Cpu *cpu) = 0;
	virtual void Cleanup() = 0;
};


// Function that retrieves an instance of the OpenXBOX module
OPENXBOX_MODULE_API typedef IOpenXBOXCPUModule* (*CreateCPUModuleFunc)();

// CPU module information exposed to the emulator core.
// This struct MUST remain backwards-compatible -- existing fields MUST NOT be
// modified, moved or removed.
struct OpenXBOXCPUModuleInfo {
	OpenXBOXModuleInfo baseInfo; // This must ALWAYS be the first entry
	int cpuModuleAPIVersion;
	const char *className;
	const char *moduleName;
	const char *moduleVersion;
	CreateCPUModuleFunc createModule;
};

#define OPENXBOX_CPU_MODULE(classType, moduleName, moduleVersion) \
    extern "C" { \
    OPENXBOX_MODULE_EXPORT IOpenXBOXCPUModule* getCPUModule() { \
        static classType singleton;  \
        return &singleton; \
    } \
    OPENXBOX_MODULE_EXPORT OpenXBOXCPUModuleInfo oxModuleInfo = { \
		COMMON_OPENXBOX_MODULE_INFO(OX_MODULE_CPU), \
        OPENXBOX_CPU_MODULE_API_VERSION, \
        #classType, \
        moduleName, \
        moduleVersion, \
        getCPUModule, \
    }; \
    }

}
