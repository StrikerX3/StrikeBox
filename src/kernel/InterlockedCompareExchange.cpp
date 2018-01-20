#include "common.h"

/*
 * InterlockedCompareExchange
 *
 * Import Number:      51
 * Calling Convention: fastcall
 * Parameter 0:        PLONG Destination
 * Parameter 1:        LONG  ExChange
 * Parameter 2:        LONG  Comparand
 * Return Type:        LONG
 */
int Xbox::InterlockedCompareExchange()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::PLONG, Destination);
	K_INIT_ARG(XboxTypes::LONG,  ExChange);
	K_INIT_ARG(XboxTypes::LONG,  Comparand);
	XboxTypes::LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
