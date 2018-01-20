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
	K_INIT_ARG(XboxTypes::HANDLE,           IoCompletionHandle);
	K_INIT_ARG(XboxTypes::PPVOID,           KeyContext);
	K_INIT_ARG(XboxTypes::PPVOID,           ApcContext);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK, IoStatusBlock);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER,   Timeout);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
