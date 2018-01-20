#include "common.h"

/*
 * NtWaitForSingleObject
 *
 * Import Number:      233
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE         Handle
 * Parameter 1:        BOOLEAN        Alertable
 * Parameter 2:        PLARGE_INTEGER Timeout
 * Return Type:        NTSTATUS
 */
int Xbox::NtWaitForSingleObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,         Handle);
	K_INIT_ARG(XboxTypes::BOOLEAN,        Alertable);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER, Timeout);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
