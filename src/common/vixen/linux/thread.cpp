#if defined(__linux__) || defined(LINUX)

#include "vixen/thread.h"

#include <pthread.h>

namespace vixen {

void Thread_SetName(const char *threadName) {
    pthread_setname_np(pthread_self(), threadName);
}

}

#endif // LINUX
