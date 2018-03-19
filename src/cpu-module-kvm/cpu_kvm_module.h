#pragma once

#include "openxbox/cpu.h"
#include "cpu_kvm.h"

namespace openxbox {

class KvmCPUModule: public IOpenXBOXCPUModule {
public:
    Cpu *GetCPU();
    void FreeCPU(Cpu *cpu);
    void Cleanup();
private:
    KvmCpu m_cpu;
};

}