#include "common.h"

/*
 * RtlAppendUnicodeStringToString
 *
 * Import Number:      262
 * Calling Convention: stdcall
 * Parameter 0:        PUNICODE_STRING Destination
 * Parameter 1:        PUNICODE_STRING Source
 * Return Type:        NTSTATUS
 */
int Xbox::RtlAppendUnicodeStringToString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, Destination);
	K_INIT_ARG(XboxTypes::PUNICODE_STRING, Source);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
