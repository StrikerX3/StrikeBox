#include "common.h"

/*
 * RtlGetCallersAddress
 *
 * Import Number:      288
 * Calling Convention: stdcall
 * Parameter 0:        PPVOID CallersAddress
 * Parameter 1:        PPVOID CallersCaller
 * Return Type:        VOID
 */
int Xbox::RtlGetCallersAddress()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PPVOID, CallersAddress);
	K_INIT_ARG(PPVOID, CallersCaller);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
