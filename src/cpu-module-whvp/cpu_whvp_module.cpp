#include <assert.h>

#include "vixen/cpu_module_decl.h"
#include "cpu_whvp_module.h"
#include "cpu_whvp.h"

namespace vixen {
namespace modules {
namespace cpu {

CPU_MODULE_BEGIN
CPU_MODULE_INFO(WhvpCPUModule, "Windows Hypervisor Platform CPU Module", "0.0.1")
CPU_MODULE_END

Cpu *WhvpCPUModule::GetCPU() {
    return &m_cpu;
}

void WhvpCPUModule::FreeCPU(Cpu *cpu) {
}

void WhvpCPUModule::Cleanup() {
}

}
}
}
