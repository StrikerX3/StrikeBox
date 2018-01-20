#include "common.h"

/*
 * KeSetEventBoostPriority
 *
 * Import Number:      146
 * Calling Convention: stdcall
 * Parameter 0:        PRKEVENT   Event
 * Parameter 1:        PPRKTHREAD Thread
 * Return Type:        VOID
 */
int Xbox::KeSetEventBoostPriority()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PRKEVENT,   Event);
	K_INIT_ARG(XboxTypes::PPRKTHREAD, Thread);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
