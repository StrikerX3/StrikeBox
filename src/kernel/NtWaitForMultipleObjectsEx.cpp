#include "common.h"

/*
 * NtWaitForMultipleObjectsEx
 *
 * Import Number:      235
 * Calling Convention: stdcall
 * Parameter 0:        ULONG           Count
 * Parameter 1:        HANDLE          Handles
 * Parameter 2:        WAIT_TYPE       WaitType
 * Parameter 3:        KPROCESSOR_MODE WaitMode
 * Parameter 4:        BOOLEAN         Alertable
 * Parameter 5:        PLARGE_INTEGER  Timeout
 * Return Type:        NTSTATUS
 */
int Xbox::NtWaitForMultipleObjectsEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,           Count);
	K_INIT_ARG(XboxTypes::HANDLE,          Handles);
	K_INIT_ARG(XboxTypes::WAIT_TYPE,       WaitType);
	K_INIT_ARG(XboxTypes::KPROCESSOR_MODE, WaitMode);
	K_INIT_ARG(XboxTypes::BOOLEAN,         Alertable);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER,  Timeout);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
