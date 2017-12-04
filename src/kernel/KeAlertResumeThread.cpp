#include "common.h"

/*
 * KeAlertResumeThread
 *
 * Import Number:      92
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD Thread
 * Return Type:        ULONG
 */
int Xbox::KeAlertResumeThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKTHREAD, Thread);
	ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
