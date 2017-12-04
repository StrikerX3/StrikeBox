#include "common.h"

/*
 * NtCreateIoCompletion
 *
 * Import Number:      191
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            IoCompletionHandle
 * Parameter 1:        ACCESS_MASK        DesiredAccess
 * Parameter 2:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 3:        ULONG              Count
 * Return Type:        NTSTATUS
 */
int Xbox::NtCreateIoCompletion()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PHANDLE,            IoCompletionHandle);
	K_INIT_ARG(ACCESS_MASK,        DesiredAccess);
	K_INIT_ARG(POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(ULONG,              Count);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
