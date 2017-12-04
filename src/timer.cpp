#include "timer.h"
#include <assert.h>
#include <SDL.h>

/*!
 * Constructor
 */
Timer::Timer()
: m_start(0), m_end(0)
{
    m_freq = SDL_GetPerformanceFrequency();
    assert(m_freq > 0);
}

/*!
 * Start the timer
 */
void Timer::Start()
{
    m_start = SDL_GetPerformanceCounter();
}

/*!
 * Stop the timer
 */
void Timer::Stop()
{
    m_end = SDL_GetPerformanceCounter();
}

/*!
 * Get number of milliseconds elapsed between calls made to \a Start and
 * \a Stop
 */
uint64_t Timer::GetMillisecondsElapsed()
{
    return (float)(m_end - m_start) / (float)m_freq * 1000;
}
