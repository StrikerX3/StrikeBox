#include "common.h"

/*
 * KeIsExecutingDpc
 *
 * Import Number:      121
 * Calling Convention: stdcall
 * Return Type:        BOOLEAN
 */
int Xbox::KeIsExecutingDpc()
{
	K_ENTER_STDCALL();
	XboxTypes::BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
