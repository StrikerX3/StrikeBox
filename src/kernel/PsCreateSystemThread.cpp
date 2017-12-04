#include "common.h"

/*
 * PsCreateSystemThread
 *
 * Import Number:      254
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE         ThreadHandle
 * Parameter 1:        PHANDLE         ThreadId
 * Parameter 2:        PKSTART_ROUTINE StartRoutine
 * Parameter 3:        PVOID           StartContext
 * Parameter 4:        BOOLEAN         DebuggerThread
 * Return Type:        NTSTATUS
 */
int Xbox::PsCreateSystemThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PHANDLE,         ThreadHandle);
	K_INIT_ARG(PHANDLE,         ThreadId);
	K_INIT_ARG(PKSTART_ROUTINE, StartRoutine);
	K_INIT_ARG(PVOID,           StartContext);
	K_INIT_ARG(BOOLEAN,         DebuggerThread);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
