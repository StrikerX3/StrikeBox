#include "common.h"

/*
 * KeResumeThread
 *
 * Import Number:      140
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD Thread
 * Return Type:        ULONG
 */
int Xbox::KeResumeThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PKTHREAD, Thread);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
