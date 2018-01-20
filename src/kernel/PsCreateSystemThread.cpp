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
	K_INIT_ARG(XboxTypes::PHANDLE,         ThreadHandle);
	K_INIT_ARG(XboxTypes::PHANDLE,         ThreadId);
	K_INIT_ARG(XboxTypes::PKSTART_ROUTINE, StartRoutine);
	K_INIT_ARG(XboxTypes::PVOID,           StartContext);
	K_INIT_ARG(XboxTypes::BOOLEAN,         DebuggerThread);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
