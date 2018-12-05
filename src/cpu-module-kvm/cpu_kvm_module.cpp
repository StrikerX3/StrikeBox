#include "vixen/cpu_module_decl.h"
#include "cpu_kvm_module.h"

namespace vixen {
namespace modules {
namespace cpu {

using namespace vixen::cpu;

CPU_MODULE_BEGIN
CPU_MODULE_INFO(KvmCPUModule, "KVM CPU Module", "0.0.1")
CPU_MODULE_END

Cpu *KvmCPUModule::GetCPU() {
    return &m_cpu;
}

void KvmCPUModule::FreeCPU(Cpu *cpu) {

}

void KvmCPUModule::Cleanup() {

}

}
}
}
