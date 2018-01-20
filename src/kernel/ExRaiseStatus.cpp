#include "common.h"

/*
 * ExRaiseStatus
 *
 * Import Number:      27
 * Calling Convention: stdcall
 * Parameter 0:        NTSTATUS Status
 * Return Type:        VOID
 */
int Xbox::ExRaiseStatus()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::NTSTATUS, Status);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
