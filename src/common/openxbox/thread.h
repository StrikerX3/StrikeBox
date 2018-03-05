#pragma once

#include "stdint.h"

namespace openxbox {

// TODO: thread groups

/*!
 * Opaque thread structure.
 */
struct Thread;

/*!
 * Thread function.
 */
typedef uint32_t (*ThreadFunc)(void *data);

/*!
 * Creates and starts a new thread with the given name.
 */
Thread *Thread_Create(char *name, ThreadFunc func, void *data);

/*!
 * Joins a thread, waiting for it to exit.
 * Returns the exit code when the target thread has exited.
 * The thread object is freed upon returning.
 */
uint32_t Thread_Join(Thread *thread);

/*!
 * Waits for all threads created with the API to exit.
 */
void Thread_JoinAll();

/*!
 * Terminates the current thread with the given exit code.
 * The corresponding Thread object is freed.
 * Has no effect on threads that were not created by the Thread API.
 */
void Thread_Exit(uint32_t exitCode);

/*!
 * Returns a reference to the current thread.
 */
Thread *Thread_Self();

}
