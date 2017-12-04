#include "common.h"

/*
 * PsSetCreateThreadNotifyRoutine
 *
 * Import Number:      257
 * Calling Convention: stdcall
 * Parameter 0:        PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
 * Return Type:        NTSTATUS
 */
int Xbox::PsSetCreateThreadNotifyRoutine()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PCREATE_THREAD_NOTIFY_ROUTINE, NotifyRoutine);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
