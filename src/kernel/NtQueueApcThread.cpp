#include "common.h"

/*
 * NtQueueApcThread
 *
 * Import Number:      206
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE          ThreadHandle
 * Parameter 1:        PPS_APC_ROUTINE ApcRoutine
 * Parameter 2:        PVOID           ApcArgument1
 * Parameter 3:        PVOID           ApcArgument2
 * Parameter 4:        PVOID           ApcArgument3
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueueApcThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,          ThreadHandle);
	K_INIT_ARG(XboxTypes::PPS_APC_ROUTINE, ApcRoutine);
	K_INIT_ARG(XboxTypes::PVOID,           ApcArgument1);
	K_INIT_ARG(XboxTypes::PVOID,           ApcArgument2);
	K_INIT_ARG(XboxTypes::PVOID,           ApcArgument3);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
