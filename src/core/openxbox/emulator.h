#pragma once

namespace openxbox {

/*!
 * Interface for main emulator functions.
 */
class Emulator {
public:
	virtual int RunCpu() = 0;
	virtual void Stop() = 0;
};

}
