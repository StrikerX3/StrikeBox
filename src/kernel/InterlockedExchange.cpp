#include "common.h"

/*
 * InterlockedExchange
 *
 * Import Number:      54
 * Calling Convention: fastcall
 * Parameter 0:        PLONG Target
 * Parameter 1:        LONG  Value
 * Return Type:        LONG
 */
int Xbox::InterlockedExchange()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::PLONG, Target);
	K_INIT_ARG(XboxTypes::LONG,  Value);
	XboxTypes::LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
