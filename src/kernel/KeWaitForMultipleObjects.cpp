#include "common.h"

/*
 * KeWaitForMultipleObjects
 *
 * Import Number:      158
 * Calling Convention: stdcall
 * Parameter 0:        ULONG           Count
 * Parameter 1:        PVOID           Object
 * Parameter 2:        WAIT_TYPE       WaitType
 * Parameter 3:        KWAIT_REASON    WaitReason
 * Parameter 4:        KPROCESSOR_MODE WaitMode
 * Parameter 5:        BOOLEAN         Alertable
 * Parameter 6:        PLARGE_INTEGER  Timeout
 * Parameter 7:        PKWAIT_BLOCK    WaitBlockArray
 * Return Type:        NTSTATUS
 */
int Xbox::KeWaitForMultipleObjects()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG,           Count);
	K_INIT_ARG(PVOID,           Object);
	K_INIT_ARG(WAIT_TYPE,       WaitType);
	K_INIT_ARG(KWAIT_REASON,    WaitReason);
	K_INIT_ARG(KPROCESSOR_MODE, WaitMode);
	K_INIT_ARG(BOOLEAN,         Alertable);
	K_INIT_ARG(PLARGE_INTEGER,  Timeout);
	K_INIT_ARG(PKWAIT_BLOCK,    WaitBlockArray);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
