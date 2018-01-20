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
	K_INIT_ARG(XboxTypes::ULONG, dwOp);
	K_INIT_ARG(XboxTypes::PVOID, pArgs);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
