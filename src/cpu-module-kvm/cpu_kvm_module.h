#pragma once

#include "openxbox/cpu.h"
#include "cpu_kvm.h"

namespace openxbox {
namespace modules {
namespace cpu {

using namespace openxbox::cpu;

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
