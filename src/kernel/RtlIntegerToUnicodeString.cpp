#include "common.h"

/*
 * RtlIntegerToUnicodeString
 *
 * Import Number:      293
 * Calling Convention: stdcall
 * Parameter 0:        ULONG           Value
 * Parameter 1:        ULONG           Base
 * Parameter 2:        PUNICODE_STRING String
 * Return Type:        NTSTATUS
 */
int Xbox::RtlIntegerToUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,           Value);
	K_INIT_ARG(XboxTypes::ULONG,           Base);
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, String);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
