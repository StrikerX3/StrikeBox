#include "common.h"

/*
 * NtSetIoCompletion
 *
 * Import Number:      227
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE    IoCompletionHandle
 * Parameter 1:        PVOID     KeyContext
 * Parameter 2:        PVOID     ApcContext
 * Parameter 3:        NTSTATUS  IoStatus
 * Parameter 4:        ULONG_PTR IoStatusInformation
 * Return Type:        NTSTATUS
 */
int Xbox::NtSetIoCompletion()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,    IoCompletionHandle);
	K_INIT_ARG(XboxTypes::PVOID,     KeyContext);
	K_INIT_ARG(XboxTypes::PVOID,     ApcContext);
	K_INIT_ARG(XboxTypes::NTSTATUS,  IoStatus);
	K_INIT_ARG(XboxTypes::ULONG_PTR, IoStatusInformation);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
