#pragma once

#include "vixen/module_common.h"
#include "vixen/cpu.h"

// This file specifies the interface and exported data for a CPU module.
// Follow the instructions in cpu_module_def.h to declare a CPU module.

namespace vixen {
namespace modules {
namespace cpu {

using namespace vixen::cpu;

// The CPU module API version
// Increment this if there are any ABI breaking changes or new features
const int apiVersion = 1;


// Base interface for CPU modules
class MODULE_API ICPUModule {
public:
    virtual Cpu *GetCPU() = 0;
    virtual void FreeCPU(Cpu *cpu) = 0;
    virtual void Cleanup() = 0;
};


// Function that retrieves an instance of the CPU module
MODULE_API typedef ICPUModule* (*CreateCPUModuleFunc)();



// CPU module information exported as vxnModuleInfo.
// This struct MUST remain backwards-compatible -- existing fields MUST NOT be
// modified, moved or removed.
struct Info {
    vixen::modules::Info baseInfo; // This must ALWAYS be the first entry
    APIVersion cpuModuleAPIVersion;
    const char *className;
    const char *moduleName;
    const char *moduleVersion;
    CreateCPUModuleFunc createModule;
};



// CPU module capabilities exported as vxnModuleCaps.
// This struct MUST remain backwards-compatible -- existing fields MUST NOT be
// modified, moved or removed.
struct Capabilities {
    // Supports guest debugging operations such as single stepping and breakpoints
    bool guestDebugging;
};

}
}
}

