#include "common.h"

/*
 * RtlRaiseStatus
 *
 * Import Number:      303
 * Calling Convention: stdcall
 * Parameter 0:        NTSTATUS Status
 * Return Type:        VOID
 */
int Xbox::RtlRaiseStatus()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(NTSTATUS, Status);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
