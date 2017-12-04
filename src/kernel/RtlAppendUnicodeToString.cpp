#include "common.h"

/*
 * RtlAppendUnicodeToString
 *
 * Import Number:      263
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING Destination
 * Parameter 1:        PCWSTR          Source
 * Return Type:        NTSTATUS
 */
int Xbox::RtlAppendUnicodeToString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUNICODE_STRING, Destination);
	K_INIT_ARG(PCWSTR,          Source);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
