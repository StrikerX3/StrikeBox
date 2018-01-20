#include "common.h"

/*
 * RtlFreeUnicodeString
 *
 * Import Number:      287
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING UnicodeString
 * Return Type:        VOID
 */
int Xbox::RtlFreeUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, UnicodeString);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
