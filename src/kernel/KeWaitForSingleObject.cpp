#include "common.h"

/*
 * KeWaitForSingleObject
 *
 * Import Number:      159
 * Calling Convention: stdcall
 * Parameter 0:        PVOID           Object
 * Parameter 1:        KWAIT_REASON    WaitReason
 * Parameter 2:        KPROCESSOR_MODE WaitMode
 * Parameter 3:        BOOLEAN         Alertable
 * Parameter 4:        PLARGE_INTEGER  Timeout
 * Return Type:        NTSTATUS
 */
int Xbox::KeWaitForSingleObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,           Object);
	K_INIT_ARG(KWAIT_REASON,    WaitReason);
	K_INIT_ARG(KPROCESSOR_MODE, WaitMode);
	K_INIT_ARG(BOOLEAN,         Alertable);
	K_INIT_ARG(PLARGE_INTEGER,  Timeout);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
