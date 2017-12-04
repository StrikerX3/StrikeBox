#include "common.h"

/*
 * NtQuerySemaphore
 *
 * Import Number:      214
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE                       SemaphoreHandle
 * Parameter 1:        PSEMAPHORE_BASIC_INFORMATION SemaphoreInformation
 * Return Type:        NTSTATUS
 */
int Xbox::NtQuerySemaphore()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,                       SemaphoreHandle);
	K_INIT_ARG(PSEMAPHORE_BASIC_INFORMATION, SemaphoreInformation);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
