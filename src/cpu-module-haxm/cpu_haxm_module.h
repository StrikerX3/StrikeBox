#pragma once

#include "openxbox/cpu.h"
#include "cpu_haxm.h"

namespace openxbox {
namespace modules {
namespace cpu {

using namespace openxbox::cpu;

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
