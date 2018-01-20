#include "common.h"

/*
 * XcSHAInit
 *
 * Import Number:      335
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbSHAContext
 * Return Type:        VOID
 */
int Xbox::XcSHAInit()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUCHAR, pbSHAContext);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
