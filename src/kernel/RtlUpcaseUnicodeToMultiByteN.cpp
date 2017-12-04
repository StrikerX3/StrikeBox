#include "common.h"

/*
 * RtlUpcaseUnicodeToMultiByteN
 *
 * Import Number:      315
 * Calling Convention: stdcall
 * Parameter 0:        PCHAR  MultiByteString
 * Parameter 1:        ULONG  MaxBytesInMultiByteString
 * Parameter 2:        PULONG BytesInMultiByteString
 * Parameter 3:        PWSTR  UnicodeString
 * Parameter 4:        ULONG  BytesInUnicodeString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlUpcaseUnicodeToMultiByteN()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PCHAR,  MultiByteString);
	K_INIT_ARG(ULONG,  MaxBytesInMultiByteString);
	K_INIT_ARG(PULONG, BytesInMultiByteString);
	K_INIT_ARG(PWSTR,  UnicodeString);
	K_INIT_ARG(ULONG,  BytesInUnicodeString);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
