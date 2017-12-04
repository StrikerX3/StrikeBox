#include "common.h"

/*
 * RtlCompareString
 *
 * Import Number:      270
 * Calling Convention: stdcall
 * Parameter 0:        PSTRING String1
 * Parameter 1:        PSTRING String2
 * Parameter 2:        BOOLEAN CaseInSensitive
 * Return Type:        LONG
 */
int Xbox::RtlCompareString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PSTRING, String1);
	K_INIT_ARG(PSTRING, String2);
	K_INIT_ARG(BOOLEAN, CaseInSensitive);
	LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
