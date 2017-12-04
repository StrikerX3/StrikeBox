#include "common.h"

/*
 * NtSignalAndWaitForSingleObjectEx
 *
 * Import Number:      230
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE          SignalHandle
 * Parameter 1:        HANDLE          WaitHandle
 * Parameter 2:        KPROCESSOR_MODE WaitMode
 * Parameter 3:        BOOLEAN         Alertable
 * Parameter 4:        PLARGE_INTEGER  Timeout
 * Return Type:        NTSTATUS
 */
int Xbox::NtSignalAndWaitForSingleObjectEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,          SignalHandle);
	K_INIT_ARG(HANDLE,          WaitHandle);
	K_INIT_ARG(KPROCESSOR_MODE, WaitMode);
	K_INIT_ARG(BOOLEAN,         Alertable);
	K_INIT_ARG(PLARGE_INTEGER,  Timeout);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
