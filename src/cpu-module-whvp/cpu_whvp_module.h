#pragma once

#include "openxbox/cpu.h"
#include "cpu_whvp.h"

namespace openxbox {

class WhvpCPUModule : public IOpenXBOXCPUModule {
public:
	Cpu *GetCPU();
	void FreeCPU(Cpu *cpu);
	void Cleanup();
private:
	WhvpCpu m_cpu;
};

}
