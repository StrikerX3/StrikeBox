#include "common.h"

/*
 * RtlDowncaseUnicodeString
 *
 * Import Number:      276
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING DestinationString
 * Parameter 1:        PUNICODE_STRING SourceString
 * Parameter 2:        BOOLEAN         AllocateDestinationString
 * Return Type:        NTSTATUS
 */
int Xbox::RtlDowncaseUnicodeString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, DestinationString);
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, SourceString);
	K_INIT_ARG(XboxTypes::BOOLEAN,         AllocateDestinationString);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
