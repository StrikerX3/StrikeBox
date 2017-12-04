#include "common.h"

/*
 * InterlockedIncrement
 *
 * Import Number:      53
 * Calling Convention: fastcall
 * Parameter 0:        PLONG Addend
 * Return Type:        LONG
 */
int Xbox::InterlockedIncrement()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(PLONG, Addend);
	LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
