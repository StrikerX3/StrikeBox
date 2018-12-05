#pragma once

#include "vixen/cpu.h"
#include "cpu_haxm.h"

namespace vixen {
namespace modules {
namespace cpu {

using namespace vixen::cpu;

class HaxmCPUModule : public ICPUModule {
public:
    Cpu *GetCPU();
    void FreeCPU(Cpu *cpu);
    void Cleanup();
private:
    HaxmCpu m_cpu;
};

}
}
}
