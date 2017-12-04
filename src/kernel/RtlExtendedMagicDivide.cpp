#include "common.h"

/*
 * RtlExtendedMagicDivide
 *
 * Import Number:      283
 * Calling Convention: stdcall
 * Parameter 0:        LARGE_INTEGER Dividend
 * Parameter 1:        LARGE_INTEGER MagicDivisor
 * Parameter 2:        CCHAR         ShiftCount
 * Return Type:        LARGE_INTEGER
 */
int Xbox::RtlExtendedMagicDivide()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(LARGE_INTEGER, Dividend);
	K_INIT_ARG(LARGE_INTEGER, MagicDivisor);
	K_INIT_ARG(CCHAR,         ShiftCount);
	LARGE_INTEGER rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
