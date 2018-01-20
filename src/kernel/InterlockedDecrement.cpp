#include "common.h"

/*
 * InterlockedDecrement
 *
 * Import Number:      52
 * Calling Convention: fastcall
 * Parameter 0:        PLONG Addend
 * Return Type:        LONG
 */
int Xbox::InterlockedDecrement()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::PLONG, Addend);
	XboxTypes::LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
