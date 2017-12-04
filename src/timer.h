#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/*!
 * Simple timer abstraction class for high-precision timing events
 */
class Timer {
protected:
    uint64_t m_freq;
    uint64_t m_start;
    uint64_t m_end;

public:
    Timer();
    void Start();
    void Stop();
    uint64_t GetMillisecondsElapsed();
};

#endif
