#include "common.h"

/*
 * NtClose
 *
 * Import Number:      187
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE Handle
 * Return Type:        NTSTATUS
 */
int Xbox::NtClose()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE, Handle);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
