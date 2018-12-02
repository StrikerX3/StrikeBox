#pragma once

#include "openxbox/cpu_module.h"

// This file provides helper macros to declare a CPU module.
//
// How to declare a CPU module:
// - Include this file
// - Create a class that implements ICPUModule
// - At the top of the .cpp file, create the module definition block:
//     CPU_MODULE_BEGIN
//     CPU_MODULE_INFO(<class>, "<name>", "<version>")
//     CPU_MODULE_CAPS.<cap1>().<cap2>();
//     CPU_MODULE_END
//   where:
//     <class> is the name of the class that implements ICPUModule;
//     <name> is the display name of the module;
//     <version> is the version of the module; and
//     <cap1> and <cap2> are methods of the CapabilitiesBuilder struct.
//
// CPU_MODULE_INFO is required and must be specified exactly once.
//
// CPU_MODULE_CAPS is optional and may be specified at most once.
// Use method chaining to specify multiple capabilities. Invoking the same
// method twice has no additional effect.


namespace openxbox {
namespace modules {
namespace cpu {

#define CPU_MODULE_BEGIN extern "C" {
#define CPU_MODULE_END }

// Declare the CPU module information
// - classType is the name of the class that implements ICPUModule
// - moduleName is the name of the module
// - moduleVersion is the version string of the module
#define CPU_MODULE_INFO(classType, moduleName, moduleVersion) \
    MODULE_EXPORT ICPUModule* getCPUModule() { \
        static classType singleton;  \
        return &singleton; \
    } \
    MODULE_EXPORT Info oxModuleInfo = { \
		COMMON_MODULE_INFO(TYPE_CPU), \
        apiVersion, \
        #classType, \
        moduleName, \
        moduleVersion, \
        getCPUModule, \
    };


// Helper struct for building the CPU module capabilities
struct CapabilitiesBuilder {
    Capabilities caps;
    CapabilitiesBuilder& guestDebugging() { caps.guestDebugging = true; return *this; }
    operator Capabilities() { return caps; }
};


// Declare the CPU module capabilities
#define CPU_MODULE_CAPS \
    MODULE_EXPORT Capabilities oxModuleCaps = CapabilitiesBuilder()

}
}
}
