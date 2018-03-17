
#include "openxbox/cpu_module.h"
#include "cpu_kvm_module.h"
#include "cpu_kvm.h"

namespace openxbox {

OPENXBOX_CPU_MODULE(KvmCPUModule, "KVM CPU Module", "0.0.1");

Cpu *KvmCPUModule::GetCPU() {
    return &m_cpu;
}

void KvmCPUModule::FreeCPU(Cpu *cpu) {

}

void KvmCPUModule::Cleanup() {

}

}