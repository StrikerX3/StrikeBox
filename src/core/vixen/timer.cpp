#include "timer.h"
#include <assert.h>

namespace vixen {

/*!
 * Constructor
 */
Timer::Timer() {
}

/*!
 * Start the timer
 */
void Timer::Start() {
    m_start = high_resolution_clock::now();
}

/*!
 * Stop the timer
 */
void Timer::Stop() {
    m_end = high_resolution_clock::now();
}

/*!
 * Get number of milliseconds elapsed between calls made to \a Start and
 * \a Stop
 */
uint64_t Timer::GetMillisecondsElapsed() {
    return duration_cast<milliseconds>(m_end - m_start).count();
}

}
