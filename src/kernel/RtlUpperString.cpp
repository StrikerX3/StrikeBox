#include "common.h"

/*
 * RtlUpperString
 *
 * Import Number:      317
 * Calling Convention: stdcall
 * Parameter 0:        PSTRING DestinationString
 * Parameter 1:        PSTRING SourceString
 * Return Type:        VOID
 */
int Xbox::RtlUpperString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PSTRING, DestinationString);
	K_INIT_ARG(PSTRING, SourceString);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
