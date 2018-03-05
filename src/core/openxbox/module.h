#pragma once

#include <string>
#include <vector>

#include "openxbox/cpu_module.h"
#include "openxbox/shlibrary.h"

namespace openxbox {

// ----- Module load status ---------------------------------------------------

/*!
 * Indicates the result of attempting to load a module.
 */
typedef int32_t ModuleLoadStatus;


// The module was loaded successfully
const ModuleLoadStatus kModuleLoadSuccess = 0;

// The module could not be loaded because of a version mismatch
const ModuleLoadStatus kModuleLoadAPIVersionMismatch = 0x80000001;

// The module instantiation failed
const ModuleLoadStatus kModuleInstantiationFailed = 0x80000002;

// The module library failed to be loaded
const ModuleLoadStatus kModuleLibraryLoadFailed = 0xF0000001;

// The loaded library does not expose a valid module interface
const ModuleLoadStatus kModuleInvalidLibrary = 0xF0000002;

// The module type does not match the expected type
const ModuleLoadStatus kModuleInvalidType = 0xF0000003;

// ----- Basic module functions -----------------------------------------------

struct CPUModuleInstance {
	SharedLibrary *library;
	IOpenXBOXCPUModule *cpuModule;
	~CPUModuleInstance();
};

/*!
 * Loads a CPU module from the supplied library into the specified instance.
 */
ModuleLoadStatus LoadCPUModule(std::wstring libraryPath, CPUModuleInstance *instance);

// ----- Module information ---------------------------------------------------

struct CPUModuleInfo {
	std::wstring libraryPath;
	std::string moduleName;
	std::string moduleVersion;
};

// ----- Module repository ----------------------------------------------------

/*!
 * A repository of OpenXBOX modules.
 */
class ModuleRepository {
public:
	/*!
	 * Populates the module repository by enumerating modules in the specified
	 * path. By default it looks in the modules folder and its subdirectories.
	 */
	void Enumerate(std::wstring path = L"modules");

	std::vector<CPUModuleInfo>& GetCPUModules() { return m_cpuModules; }
private:
	std::vector<CPUModuleInfo> m_cpuModules;
};

}