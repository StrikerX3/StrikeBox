#include "common.h"

/*
 * NtReleaseSemaphore
 *
 * Import Number:      222
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE SemaphoreHandle
 * Parameter 1:        LONG   ReleaseCount
 * Parameter 2:        PLONG  PreviousCount
 * Return Type:        NTSTATUS
 */
int Xbox::NtReleaseSemaphore()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE, SemaphoreHandle);
	K_INIT_ARG(XboxTypes::LONG,   ReleaseCount);
	K_INIT_ARG(XboxTypes::PLONG,  PreviousCount);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
