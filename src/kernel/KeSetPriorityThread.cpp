#include "common.h"

/*
 * KeSetPriorityThread
 *
 * Import Number:      148
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD  Thread
 * Parameter 1:        KPRIORITY Priority
 * Return Type:        KPRIORITY
 */
int Xbox::KeSetPriorityThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PKTHREAD,  Thread);
	K_INIT_ARG(XboxTypes::KPRIORITY, Priority);
	XboxTypes::KPRIORITY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
