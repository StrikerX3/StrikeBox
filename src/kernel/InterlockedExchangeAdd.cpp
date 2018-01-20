#include "common.h"

/*
 * InterlockedExchangeAdd
 *
 * Import Number:      55
 * Calling Convention: fastcall
 * Parameter 0:        PLONG Addend
 * Parameter 1:        LONG  Increment
 * Return Type:        LONG
 */
int Xbox::InterlockedExchangeAdd()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::PLONG, Addend);
	K_INIT_ARG(XboxTypes::LONG,  Increment);
	XboxTypes::LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
