#include "common.h"

/*
 * RtlAppendStringToString
 *
 * Import Number:      261
 * Calling Convention: stdcall
 * Parameter 0:        PSTRING Destination
 * Parameter 1:        PSTRING Source
 * Return Type:        NTSTATUS
 */
int Xbox::RtlAppendStringToString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PSTRING, Destination);
	K_INIT_ARG(XboxTypes::PSTRING, Source);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
