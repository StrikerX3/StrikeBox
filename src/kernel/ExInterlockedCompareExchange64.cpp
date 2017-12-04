#include "common.h"

/*
 * ExInterlockedCompareExchange64
 *
 * Import Number:      21
 * Calling Convention: fastcall
 * Parameter 0:        PLONGLONG Destination
 * Parameter 1:        PLONGLONG Exchange
 * Parameter 2:        PLONGLONG Comparand
 * Return Type:        LONGLONG
 */
int Xbox::ExInterlockedCompareExchange64()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(PLONGLONG, Destination);
	K_INIT_ARG(PLONGLONG, Exchange);
	K_INIT_ARG(PLONGLONG, Comparand);
	LONGLONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
