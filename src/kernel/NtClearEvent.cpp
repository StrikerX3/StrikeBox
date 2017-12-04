#include "common.h"

/*
 * NtClearEvent
 *
 * Import Number:      186
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE EventHandle
 * Return Type:        NTSTATUS
 */
int Xbox::NtClearEvent()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE, EventHandle);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
