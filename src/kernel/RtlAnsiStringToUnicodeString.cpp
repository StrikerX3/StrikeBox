#include "common.h"

/*
 * RtlAnsiStringToUnicodeString
 *
 * Import Number:      260
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING DestinationString
 * Parameter 1:        PSTRING         SourceString
 * Parameter 2:        BOOLEAN         AllocateDestinationString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlAnsiStringToUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUNICODE_STRING, DestinationString);
	K_INIT_ARG(PSTRING,         SourceString);
	K_INIT_ARG(BOOLEAN,         AllocateDestinationString);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
