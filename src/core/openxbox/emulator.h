#pragma once

#include <openxbox/poller.h>

namespace openxbox {

/*!
 * Interface for main emulator functions.
 */
class Emulator {
public:
	virtual int RunCpu() = 0;
	virtual void Stop() = 0;

    virtual void AddPoller(PollFunc pollFunc, void *data) = 0;
    virtual void RemovePoller(PollFunc pollFunc, void *data) = 0;
};

}
