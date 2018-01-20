#include "common.h"

/*
 * NtWaitForSingleObjectEx
 *
 * Import Number:      234
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE          Handle
 * Parameter 1:        KPROCESSOR_MODE WaitMode
 * Parameter 2:        BOOLEAN         Alertable
 * Parameter 3:        PLARGE_INTEGER  Timeout
 * Return Type:        NTSTATUS
 */
int Xbox::NtWaitForSingleObjectEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,          Handle);
	K_INIT_ARG(XboxTypes::KPROCESSOR_MODE, WaitMode);
	K_INIT_ARG(XboxTypes::BOOLEAN,         Alertable);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER,  Timeout);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
