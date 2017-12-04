#include "common.h"

/*
 * IoSetIoCompletion
 *
 * Import Number:      79
 * Calling Convention: stdcall
 * Parameter 0:        PVOID     IoCompletion
 * Parameter 1:        PVOID     KeyContext
 * Parameter 2:        PVOID     ApcContext
 * Parameter 3:        NTSTATUS  IoStatus
 * Parameter 4:        ULONG_PTR IoStatusInformation
 * Return Type:        NTSTATUS
 */
int Xbox::IoSetIoCompletion()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,     IoCompletion);
	K_INIT_ARG(PVOID,     KeyContext);
	K_INIT_ARG(PVOID,     ApcContext);
	K_INIT_ARG(NTSTATUS,  IoStatus);
	K_INIT_ARG(ULONG_PTR, IoStatusInformation);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
