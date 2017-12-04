#include "common.h"

/*
 * KeTestAlertThread
 *
 * Import Number:      155
 * Calling Convention: stdcall
 * Parameter 0:        KPROCESSOR_MODE ProcessorMode
 * Return Type:        BOOLEAN
 */
int Xbox::KeTestAlertThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(KPROCESSOR_MODE, ProcessorMode);
	BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
