#pragma once

#include "vixen/cpu.h"
#include "cpu_kvm.h"

namespace vixen {
namespace modules {
namespace cpu {

using namespace vixen::cpu;

class KvmCPUModule : public ICPUModule {
public:
    Cpu *GetCPU();
    void FreeCPU(Cpu *cpu);
    void Cleanup();
private:
    KvmCpu m_cpu;
};

}
}
}
