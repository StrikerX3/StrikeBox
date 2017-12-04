#include "common.h"

/*
 * NtQuerySymbolicLinkObject
 *
 * Import Number:      215
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE         LinkHandle
 * Parameter 1:        POBJECT_STRING LinkTarget
 * Parameter 2:        PULONG         ReturnedLength
 * Return Type:        NTSTATUS
 */
int Xbox::NtQuerySymbolicLinkObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,         LinkHandle);
	K_INIT_ARG(POBJECT_STRING, LinkTarget);
	K_INIT_ARG(PULONG,         ReturnedLength);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
