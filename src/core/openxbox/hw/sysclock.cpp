#include "openxbox/hw/sysclock.h"

#include <chrono>
#include <thread>

namespace openxbox {

SystemClock::SystemClock(Cpu *cpu, float tickRate)
	: m_cpu(cpu)
	, m_tickRate(tickRate)
	, m_running(false)
{
}

void SystemClock::Run() {
	using namespace std::chrono;
	auto nextStop = high_resolution_clock::now();
	auto interval = duration<long long, std::ratio<1, 1000000>>((long long)(1000000.0f / m_tickRate));
	
	m_running = true;
	while (m_running) {
        m_cpu->Interrupt(0x30);
     
		nextStop += interval;
		std::this_thread::sleep_until(nextStop);
    }
}

void SystemClock::Stop() {
	m_running = false;
}

}