#include "common.h"

/*
 * KeBoostPriorityThread
 *
 * Import Number:      94
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD  Thread
 * Parameter 1:        KPRIORITY Increment
 * Return Type:        VOID
 */
int Xbox::KeBoostPriorityThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKTHREAD,  Thread);
	K_INIT_ARG(KPRIORITY, Increment);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
