#include "common.h"

/*
 * XcCryptService
 *
 * Import Number:      350
 * Calling Convention: stdcall
 * Parameter 0:        ULONG dwOp
 * Parameter 1:        PVOID pArgs
 * Return Type:        ULONG
 */
int Xbox::XcCryptService()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG, dwOp);
	K_INIT_ARG(PVOID, pArgs);
	ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
