#include "common.h"

/*
 * RtlCompareUnicodeString
 *
 * Import Number:      271
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING String1
 * Parameter 1:        PUNICODE_STRING String2
 * Parameter 2:        BOOLEAN         CaseInSensitive
 * Return Type:        LONG
 */
int Xbox::RtlCompareUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, String1);
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, String2);
	K_INIT_ARG(XboxTypes::BOOLEAN,         CaseInSensitive);
	XboxTypes::LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
