#include "common.h"

/*
 * NtRemoveIoCompletion
 *
 * Import Number:      223
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE           IoCompletionHandle
 * Parameter 1:        PPVOID           KeyContext
 * Parameter 2:        PPVOID           ApcContext
 * Parameter 3:        PIO_STATUS_BLOCK IoStatusBlock
 * Parameter 4:        PLARGE_INTEGER   Timeout
 * Return Type:        NTSTATUS
 */
int Xbox::NtRemoveIoCompletion()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,           IoCompletionHandle);
	K_INIT_ARG(PPVOID,           KeyContext);
	K_INIT_ARG(PPVOID,           ApcContext);
	K_INIT_ARG(PIO_STATUS_BLOCK, IoStatusBlock);
	K_INIT_ARG(PLARGE_INTEGER,   Timeout);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
