#include "common.h"

/*
 * RtlExtendedLargeIntegerDivide
 *
 * Import Number:      282
 * Calling Convention: stdcall
 * Parameter 0:        LARGE_INTEGER Dividend
 * Parameter 1:        ULONG         Divisor
 * Parameter 2:        PULONG        Remainder
 * Return Type:        LARGE_INTEGER
 */
int Xbox::RtlExtendedLargeIntegerDivide()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::LARGE_INTEGER, Dividend);
	K_INIT_ARG(XboxTypes::ULONG,         Divisor);
	K_INIT_ARG(XboxTypes::PULONG,        Remainder);
	XboxTypes::LARGE_INTEGER rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
