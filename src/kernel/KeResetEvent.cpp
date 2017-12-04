#include "common.h"

/*
 * KeResetEvent
 *
 * Import Number:      138
 * Calling Convention: stdcall
 * Parameter 0:        PRKEVENT Event
 * Return Type:        LONG
 */
int Xbox::KeResetEvent()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PRKEVENT, Event);
	LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
