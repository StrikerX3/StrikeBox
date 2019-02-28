#include "strikebox/thread.h"

#include <pthread.h>

namespace strikebox {

void Thread_SetName(const char *threadName) {
    pthread_setname_np(pthread_self(), threadName);
}

}
