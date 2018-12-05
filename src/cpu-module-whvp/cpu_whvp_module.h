#pragma once

#include "vixen/cpu.h"
#include "cpu_whvp.h"

namespace vixen {
namespace modules {
namespace cpu {

class WhvpCPUModule : public ICPUModule {
public:
    Cpu *GetCPU();
    void FreeCPU(Cpu *cpu);
    void Cleanup();
private:
    WhvpCpu m_cpu;
};

}
}
}
