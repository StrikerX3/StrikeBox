#include "common.h"

/*
 * RtlUnicodeStringToInteger
 *
 * Import Number:      309
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING String
 * Parameter 1:        ULONG           Base
 * Parameter 2:        PULONG          Value
 * Return Type:        NTSTATUS
 */
int Xbox::RtlUnicodeStringToInteger()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUNICODE_STRING, String);
	K_INIT_ARG(ULONG,           Base);
	K_INIT_ARG(PULONG,          Value);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
