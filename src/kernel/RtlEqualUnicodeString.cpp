#include "common.h"

/*
 * RtlEqualUnicodeString
 *
 * Import Number:      280
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING String1
 * Parameter 1:        PUNICODE_STRING String2
 * Parameter 2:        BOOLEAN         CaseInSensitive
 * Return Type:        BOOLEAN
 */
int Xbox::RtlEqualUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUNICODE_STRING, String1);
	K_INIT_ARG(PUNICODE_STRING, String2);
	K_INIT_ARG(BOOLEAN,         CaseInSensitive);
	BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
