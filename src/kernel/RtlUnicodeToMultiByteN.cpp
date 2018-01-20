#include "common.h"

/*
 * RtlUnicodeToMultiByteN
 *
 * Import Number:      310
 * Calling Convention: stdcall
 * Parameter 0:        PCHAR  MultiByteString
 * Parameter 1:        ULONG  MaxBytesInMultiByteString
 * Parameter 2:        PULONG BytesInMultiByteString
 * Parameter 3:        PWSTR  UnicodeString
 * Parameter 4:        ULONG  BytesInUnicodeString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlUnicodeToMultiByteN()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PCHAR,  MultiByteString);
	K_INIT_ARG(XboxTypes::ULONG,  MaxBytesInMultiByteString);
	K_INIT_ARG(XboxTypes::PULONG, BytesInMultiByteString);
	K_INIT_ARG(XboxTypes::PWSTR,  UnicodeString);
	K_INIT_ARG(XboxTypes::ULONG,  BytesInUnicodeString);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
