#include "common.h"

/*
 * RtlUpcaseUnicodeString
 *
 * Import Number:      314
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING  DestinationString
 * Parameter 1:        PCUNICODE_STRING SourceString
 * Parameter 2:        BOOLEAN          AllocateDestinationString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlUpcaseUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUNICODE_STRING,  DestinationString);
	K_INIT_ARG(PCUNICODE_STRING, SourceString);
	K_INIT_ARG(BOOLEAN,          AllocateDestinationString);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
