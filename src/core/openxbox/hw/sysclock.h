#pragma once

#include "openxbox/cpu.h"

namespace openxbox {

/*!
 * Represents the Xbox system clock.
 *
 * The system clock runs at 1000 ticks per second, generating IRQ 0 which
 * corresponds to interrupt vector 0x30.
 *
 * The emulated system clock can be readjusted to tick at any rate, for
 * debugging, testing and performance purposes. By default, it ticks 1000 times
 * per second, just like the real hardware.
 */
class SystemClock {
public:
	/*!
	 * Constructs a system clock that ticks at the specified rate, generating
	 * interrupts against the supplied CPU.
	 */
	SystemClock(Cpu *cpu, float tickRate = 1000.0f);

	/*!
	 * Runs the system clock.
	 */
	void Run();

	/*!
	 * Stops the system clock.
	 */
	void Stop();

private:
	Cpu *m_cpu;
	float m_tickRate;
	bool m_running;
};

}