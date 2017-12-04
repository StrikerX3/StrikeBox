#include "common.h"

/*
 * NtYieldExecution
 *
 * Import Number:      238
 * Calling Convention: stdcall
 * Return Type:        NTSTATUS
 */
int Xbox::NtYieldExecution()
{
	K_ENTER_STDCALL();
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
