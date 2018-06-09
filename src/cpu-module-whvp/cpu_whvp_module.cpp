#include <assert.h>

#include "openxbox/cpu_module.h"
#include "cpu_whvp_module.h"
#include "cpu_whvp.h"

namespace openxbox {

OPENXBOX_CPU_MODULE(WhvpCPUModule, "Windows Hypervisor Platform CPU Module", "0.0.1");

Cpu *WhvpCPUModule::GetCPU() {
	return &m_cpu;
}

void WhvpCPUModule::FreeCPU(Cpu *cpu) {
}

void WhvpCPUModule::Cleanup() {
}

}
