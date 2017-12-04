#include "common.h"

/*
 * KeAlertThread
 *
 * Import Number:      93
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD        Thread
 * Parameter 1:        KPROCESSOR_MODE ProcessorMode
 * Return Type:        BOOLEAN
 */
int Xbox::KeAlertThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKTHREAD,        Thread);
	K_INIT_ARG(KPROCESSOR_MODE, ProcessorMode);
	BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
