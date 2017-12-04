#include "common.h"

/*
 * HalIsResetOrShutdownPending
 *
 * Import Number:      358
 * Calling Convention: stdcall
 * Return Type:        BOOLEAN
 */
int Xbox::HalIsResetOrShutdownPending()
{
	K_ENTER_STDCALL();
	BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
