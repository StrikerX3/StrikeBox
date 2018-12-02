#pragma once

#include "openxbox/cpu.h"
#include "cpu_whvp.h"

namespace openxbox {
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
