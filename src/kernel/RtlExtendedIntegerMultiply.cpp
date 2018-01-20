#include "common.h"

/*
 * RtlExtendedIntegerMultiply
 *
 * Import Number:      281
 * Calling Convention: stdcall
 * Parameter 0:        LARGE_INTEGER Multiplicand
 * Parameter 1:        LONG          Multiplier
 * Return Type:        LARGE_INTEGER
 */
int Xbox::RtlExtendedIntegerMultiply()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::LARGE_INTEGER, Multiplicand);
	K_INIT_ARG(XboxTypes::LONG,          Multiplier);
	XboxTypes::LARGE_INTEGER rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
