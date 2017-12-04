#include "common.h"

/*
 * RtlUnicodeStringToAnsiString
 *
 * Import Number:      308
 * Calling Convention: stdcall
 * Parameter 0:        PSTRING         DestinationString
 * Parameter 1:        PUNICODE_STRING SourceString
 * Parameter 2:        BOOLEAN         AllocateDestinationString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlUnicodeStringToAnsiString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PSTRING,         DestinationString);
	K_INIT_ARG(PUNICODE_STRING, SourceString);
	K_INIT_ARG(BOOLEAN,         AllocateDestinationString);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
