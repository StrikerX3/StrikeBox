#include "invoke_later.h"

#include <thread>

namespace vixen {

void InvokeLaterThreadFunc(void *data) {
    ((InvokeLater *)data)->Run();
}

InvokeLater::InvokeLater(InvokeLaterFunc func, void *userData)
    : m_func(func)
    , m_userData(userData)
{
    m_running = false;
    m_primed = false;
    m_thread = nullptr;
}

InvokeLater::~InvokeLater() {
    Stop();
    if (m_thread != nullptr) {
        m_thread->join();
    }
    delete m_thread;
}

void InvokeLater::Start() {
    if (m_running) {
        return;
    }

    m_running = true;
    m_thread = new std::thread(InvokeLaterThreadFunc, this);
}

void InvokeLater::Stop() {
    m_running = false;
    Cancel();
}

void InvokeLater::Set(std::chrono::time_point<std::chrono::high_resolution_clock>& expiration) {
    if (m_func == nullptr) {
        return;
    }

    Cancel();
    m_targetExpiration = expiration;
    m_primed = true;
    m_setupCond.notify_one();
}

void InvokeLater::Cancel() {
    m_primed = false;
    {
        std::unique_lock<std::mutex> lock(m_setupMutex);
        m_setupCond.notify_one();
    }
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.notify_one();
    }
}

void InvokeLater::Run() {
    while (m_running) {
        // Wait for a timer to be set up
        {
            std::unique_lock<std::mutex> lock(m_setupMutex);
            m_setupCond.wait(lock);
        }

        // Wait until the expiration time is reached
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait_until(lock, m_targetExpiration);
        }

        if (!m_running) {
            break;
        }

        if (m_primed && m_func != nullptr) {
            m_func(m_userData);
        }
    }
}

}

