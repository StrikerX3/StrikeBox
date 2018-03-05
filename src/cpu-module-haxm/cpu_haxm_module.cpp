#include <assert.h>

#include "openxbox/cpu_module.h"
#include "cpu_haxm_module.h"
#include "cpu_haxm.h"

namespace openxbox {

OPENXBOX_CPU_MODULE(HaxmCPUModule, "Intel HAXM CPU Module", "0.0.1");

Cpu *HaxmCPUModule::GetCPU() {
	return &m_cpu;
}

void HaxmCPUModule::FreeCPU(Cpu *cpu) {
}

void HaxmCPUModule::Cleanup() {
}

}
