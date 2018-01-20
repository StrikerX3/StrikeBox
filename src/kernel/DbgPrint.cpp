#include "common.h"

/*
 * DbgPrint
 *
 * Import Number:      8
 * Calling Convention: cdecl
 * Parameter 0:        PCH Format
 * Return Type:        ULONG
 */
int Xbox::DbgPrint()
{
	K_ENTER_CDECL();
	K_INIT_ARG(XboxTypes::PCH, Format);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
