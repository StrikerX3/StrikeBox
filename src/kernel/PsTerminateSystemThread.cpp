#include "common.h"

/*
 * PsTerminateSystemThread
 *
 * Import Number:      258
 * Calling Convention: stdcall
 * Parameter 0:        NTSTATUS ExitStatus
 * Return Type:        VOID
 */
int Xbox::PsTerminateSystemThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(NTSTATUS, ExitStatus);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
