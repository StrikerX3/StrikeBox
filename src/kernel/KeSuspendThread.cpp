#include "common.h"

/*
 * KeSuspendThread
 *
 * Import Number:      152
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD Thread
 * Return Type:        ULONG
 */
int Xbox::KeSuspendThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PKTHREAD, Thread);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
