#if defined(__linux__) || defined(LINUX)

#include "openxbox/thread.h"

#include <pthread.h>

namespace openxbox {

void Thread_SetName(const char *threadName) {
    pthread_setname_np(pthread_self(), threadName);
}

}

#endif // LINUX
