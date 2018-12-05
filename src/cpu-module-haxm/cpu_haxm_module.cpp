#include <assert.h>

#include "vixen/cpu_module_decl.h"
#include "cpu_haxm_module.h"
#include "cpu_haxm.h"

namespace vixen {
namespace modules {
namespace cpu {

CPU_MODULE_BEGIN
CPU_MODULE_INFO(HaxmCPUModule, "Intel HAXM CPU Module", "0.0.1")
CPU_MODULE_CAPS.guestDebugging();
CPU_MODULE_END

Cpu *HaxmCPUModule::GetCPU() {
    return &m_cpu;
}

void HaxmCPUModule::FreeCPU(Cpu *cpu) {
}

void HaxmCPUModule::Cleanup() {
}

}
}
}
