#include "common.h"

/*
 * RtlUnicodeToMultiByteSize
 *
 * Import Number:      311
 * Calling Convention: stdcall
 * Parameter 0:        PULONG BytesInMultiByteString
 * Parameter 1:        PWSTR  UnicodeString
 * Parameter 2:        ULONG  BytesInUnicodeString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlUnicodeToMultiByteSize()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PULONG, BytesInMultiByteString);
	K_INIT_ARG(XboxTypes::PWSTR,  UnicodeString);
	K_INIT_ARG(XboxTypes::ULONG,  BytesInUnicodeString);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
