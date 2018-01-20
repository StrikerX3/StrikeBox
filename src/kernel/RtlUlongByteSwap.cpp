#include "common.h"

/*
 * RtlUlongByteSwap
 *
 * Import Number:      307
 * Calling Convention: fastcall
 * Parameter 0:        ULONG Source
 * Return Type:        ULONG
 */
int Xbox::RtlUlongByteSwap()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::ULONG, Source);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
