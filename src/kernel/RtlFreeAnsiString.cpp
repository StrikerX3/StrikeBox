#include "common.h"

/*
 * RtlFreeAnsiString
 *
 * Import Number:      286
 * Calling Convention: stdcall
 * Parameter 0:        PANSI_STRING AnsiString
 * Return Type:        VOID
 */
int Xbox::RtlFreeAnsiString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PANSI_STRING, AnsiString);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
