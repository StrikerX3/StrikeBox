#pragma once

#include <stdint.h>
#include <chrono>

using namespace std::chrono;

namespace openxbox {

/*!
 * Simple timer abstraction class for high-precision timing events
 */
class Timer {
protected:
	high_resolution_clock::time_point m_start;
	high_resolution_clock::time_point m_end;

public:
	Timer();
	void Start();
	void Stop();
	uint64_t GetMillisecondsElapsed();
};

}
