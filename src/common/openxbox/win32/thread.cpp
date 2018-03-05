#ifdef _WIN32

#include "openxbox/thread.h"

#include <Windows.h>
#include <map>
#include <cassert>

namespace openxbox {

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

struct Thread {
	HANDLE hThread;
	DWORD dwThreadId;
	uint32_t refCount;
};

static std::map<DWORD, Thread *> g_threads;

inline void _Thread_Add(Thread *thread) {
	thread->refCount = 0;
	g_threads[thread->dwThreadId] = thread;
}

inline void _Thread_Ref(Thread *thread) {
	thread->refCount++;
}

inline void _Thread_Unref(Thread *thread) {
	if (thread == nullptr) {
		return;
	}
	thread->refCount--;
	if (thread->refCount == 0) {
		g_threads.erase(thread->dwThreadId);
		delete thread;
	}
}

struct ThreadParams {
	ThreadFunc func;
	void *data;
};

DWORD WINAPI Thread_Routine(LPVOID param) {
	ThreadParams *params = (ThreadParams *)param;

	// Make a local copy so we can free the memory
	ThreadParams localParams = *params;
	delete params;

	DWORD result = (DWORD)localParams.func(localParams.data);
	_Thread_Unref(Thread_Self());
	return result;
}

Thread *Thread_Create(char *name, ThreadFunc func, void *data) {
	assert(func != nullptr);
	ThreadParams *params = new ThreadParams{ func, data };

	Thread *thread = new Thread;
	thread->hThread = CreateThread(NULL, 0, Thread_Routine, params, CREATE_SUSPENDED, &thread->dwThreadId);
	if (thread->hThread == INVALID_HANDLE_VALUE) {
		delete thread;
		return nullptr;
	}
	_Thread_Add(thread);
	_Thread_Ref(thread);
	SetThreadName(thread->dwThreadId, name);
	ResumeThread(thread->hThread);

	return thread;
}

uint32_t Thread_Join(Thread *thread) {
	assert(thread != nullptr);
	_Thread_Ref(thread);

	DWORD result;
	do {
		result = WaitForSingleObject(thread->hThread, INFINITE);
		if (result == WAIT_OBJECT_0) {
			DWORD exitCode;
			if (GetExitCodeThread(thread->hThread, &exitCode)) {
				_Thread_Unref(thread);
				return exitCode;
			}
			else {
				assert(0); // FIXME: something is wrong
			}
		}
		// TODO: what to do if the wait fails?
	} while (result != WAIT_OBJECT_0);

	assert(0); // FIXME: should never reach here
	return -1;
}

void Thread_Exit(uint32_t exitCode) {
	Thread *self = Thread_Self();
	assert(self != nullptr);

	HANDLE hThread = self->hThread;
	_Thread_Unref(self);
	TerminateThread(hThread, exitCode);
}

Thread *Thread_Self() {
	DWORD dwThreadId = GetCurrentThreadId();

	return (g_threads.count(dwThreadId))
		? g_threads[dwThreadId]
		: nullptr;
}

void Thread_JoinAll() {
	std::map<DWORD, Thread *> threads(g_threads);
	
	HANDLE *hThreads = new HANDLE[threads.size()];
	int count = 0;
	for (auto it = threads.begin(); it != threads.end(); it++, count++) {
		hThreads[count] = it->second->hThread;
	}

	for (int i = 0; i < count; i += 64) {
		DWORD countMax = min(64, count - i);
		DWORD result = WaitForMultipleObjects(countMax, &hThreads[i], TRUE, INFINITE);
		// TODO: handle failures gracefully
	}

	delete[] hThreads;
}

}

#endif
