#include "common.h"

/*
 * RtlUshortByteSwap
 *
 * Import Number:      318
 * Calling Convention: fastcall
 * Parameter 0:        USHORT Source
 * Return Type:        USHORT
 */
int Xbox::RtlUshortByteSwap()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::USHORT, Source);
	XboxTypes::USHORT rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
