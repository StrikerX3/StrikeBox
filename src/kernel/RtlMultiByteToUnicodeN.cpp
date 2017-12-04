#include "common.h"

/*
 * RtlMultiByteToUnicodeN
 *
 * Import Number:      299
 * Calling Convention: stdcall
 * Parameter 0:        PWSTR  UnicodeString
 * Parameter 1:        ULONG  MaxBytesInUnicodeString
 * Parameter 2:        PULONG BytesInUnicodeString
 * Parameter 3:        PCHAR  MultiByteString
 * Parameter 4:        ULONG  BytesInMultiByteString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlMultiByteToUnicodeN()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PWSTR,  UnicodeString);
	K_INIT_ARG(ULONG,  MaxBytesInUnicodeString);
	K_INIT_ARG(PULONG, BytesInUnicodeString);
	K_INIT_ARG(PCHAR,  MultiByteString);
	K_INIT_ARG(ULONG,  BytesInMultiByteString);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
