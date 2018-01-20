#include "common.h"

/*
 * NtQueryIoCompletion
 *
 * Import Number:      212
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE                           IoCompletionHandle
 * Parameter 1:        PIO_COMPLETION_BASIC_INFORMATION IoCompletionInformation
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueryIoCompletion()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,                           IoCompletionHandle);
	K_INIT_ARG(XboxTypes::PIO_COMPLETION_BASIC_INFORMATION, IoCompletionInformation);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
