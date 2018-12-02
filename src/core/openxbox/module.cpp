#include "openxbox/util.h"
#include "openxbox/log.h"
#include "openxbox/module.h"

#include "openxbox/cpu_module.h"

namespace openxbox {

#ifdef _WIN32
static const ci_wstring kLibraryPathSuffix = L".dll";
#else
static const ci_wstring kLibraryPathSuffix = L".so";
#endif


void ModuleRepository::Enumerate(std::wstring modulePath) {
    using namespace std::experimental::filesystem;
    path pPath(absolute(modulePath));
    
    log_debug("Enumerating modules in %S\n", pPath.wstring().c_str());

    // Clear list of modules
    m_cpuModules.clear();

    // Open every shared library in the modules path and its subdirectories
    for (recursive_directory_iterator next(pPath), end; next != end; ++next) {
        auto entry = next->path();
        auto wsEntry = entry.wstring();
        ci_wstring filename = ci_wstring(entry.filename().wstring().c_str());
        
        // Found a shared library
        if (!is_directory(entry) && ends_with(filename, kLibraryPathSuffix)) {
            SharedLibrary *library;
            auto status = SharedLibrary_Load(entry.wstring(), &library);
            
            // Failed to load the library
            if (status != kLibraryLoadSuccess) {
                log_debug("Failed to load module: %S\n", filename.c_str());
                continue;
            }

            // Retrieve CPU module info
            auto moduleInfo = (openxbox::modules::Info *)library->GetExport("oxModuleInfo");
            if (moduleInfo == nullptr) {
                delete library;
                continue;
            }

            // Check if module API version matches
            if (moduleInfo->apiVersion != openxbox::modules::apiVersion) {
                log_debug("%S: Module API version mismatch: expected %d, found %d\n", wsEntry.c_str(), openxbox::modules::apiVersion, moduleInfo->apiVersion);
                delete library;
                continue;
            }

            // Handle each specific type of module
            switch (moduleInfo->type) {
            case openxbox::modules::TYPE_CPU: {
                openxbox::modules::cpu::Info *cpuModuleInfo = (openxbox::modules::cpu::Info *)moduleInfo;

                // Check if the CPU module API version matches
                if (cpuModuleInfo->cpuModuleAPIVersion == openxbox::modules::cpu::apiVersion) {
                    // Fill in module data and add it to the list
                    CPUModuleInfo info = {
                        wsEntry.c_str(),
                        cpuModuleInfo->moduleName,
                        cpuModuleInfo->moduleVersion
                    };
                    m_cpuModules.push_back(info);
                    log_debug("Found %s %s in %S\n", info.moduleName.c_str(), info.moduleVersion.c_str(), wsEntry.c_str());

                    // Retrieve CPU module capabilities
                    auto moduleCaps = (openxbox::modules::cpu::Capabilities *)library->GetExport("oxModuleCaps");
                    if (moduleCaps != nullptr) {
                        log_debug("Capabilities:\n");
                        log_debug("  Guest debugging: %s\n", ((moduleCaps->guestDebugging) ? "yes" : "no"));
                    }
                }
                else {
                    log_debug("%S: CPU module API version mismatch: expected %d, found %d\n", wsEntry.c_str(), openxbox::modules::cpu::apiVersion, cpuModuleInfo->cpuModuleAPIVersion);
                }
                break;
            }
            default: {
                log_debug("%S: Unknown module type %d\n", wsEntry.c_str());
                break;
            }
            }

            delete library;
        }
    }
}

ModuleLoadStatus LoadCPUModule(std::wstring libraryPath, CPUModuleInstance *instance) {
    SharedLibrary *library;
    auto status = SharedLibrary_Load(libraryPath, &library);
    if (status != kLibraryLoadSuccess) {
        return kModuleLibraryLoadFailed;
    }

    auto moduleInfo = (openxbox::modules::Info *)library->GetExport("oxModuleInfo");
    if (moduleInfo == nullptr) {
        delete library;
        return kModuleInvalidLibrary;
    }

    if (moduleInfo->type != openxbox::modules::TYPE_CPU) {
        delete library;
        return kModuleInvalidType;
    }
    
    auto cpuModuleInfo = (openxbox::modules::cpu::Info *)moduleInfo;

    if ((moduleInfo->apiVersion != openxbox::modules::apiVersion) ||
        (cpuModuleInfo->cpuModuleAPIVersion != openxbox::modules::cpu::apiVersion)) {
        delete library;
        return kModuleLoadAPIVersionMismatch;
    }

    openxbox::modules::cpu::ICPUModule *moduleInstance = cpuModuleInfo->createModule();
    if (moduleInstance == nullptr) {
        delete library;
        return kModuleInstantiationFailed;
    }

    instance->library = library;
    instance->cpuModule = moduleInstance;
    return kModuleLoadSuccess;
}

CPUModuleInstance::~CPUModuleInstance() {
    cpuModule->Cleanup();
    delete library;
}

}

