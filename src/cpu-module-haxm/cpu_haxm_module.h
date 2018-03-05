#pragma once

#include "openxbox/cpu.h"
#include "cpu_haxm.h"

namespace openxbox {

class HaxmCPUModule : public IOpenXBOXCPUModule {
public:
	Cpu *GetCPU();
	void FreeCPU(Cpu *cpu);
	void Cleanup();
private:
	HaxmCpu m_cpu;
};

}
