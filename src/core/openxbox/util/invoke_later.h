#pragma once

#include <chrono>
#include <mutex>
#include <condition_variable>

namespace openxbox {

/*!
 * Function to be fired when scheduled.
 */
typedef void (*InvokeLaterFunc)(void *userData);

/*!
 * An object that invokes a function at a later point in time.
 * The object can be reused multiple times.
 */
class InvokeLater {
public:
    InvokeLater(InvokeLaterFunc callback, void *userData);
    ~InvokeLater();

    /*!
     * Starts the timer thread.
     */
    void Start();

    /*!
     * Stops the timer thread.
     */
    void Stop();

    /*!
     * Sets the timer to invoke at the specified expiration time.
     */
    void Set(std::chrono::time_point<std::chrono::steady_clock>& expiration);

    /*!
     * Cancels a pending invocation.
     */
    void Cancel();

private:
    void Run();
    
    bool m_primed;
    InvokeLaterFunc m_func;
    void *m_userData;

    std::thread *m_thread;
    std::mutex m_setupMutex;
    std::condition_variable m_setupCond;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::chrono::time_point<std::chrono::steady_clock> m_targetExpiration;

    bool m_running;

    friend void InvokeLaterThreadFunc(void *data);
};

}
