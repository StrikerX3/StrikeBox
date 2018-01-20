#include "common.h"

/*
 * XcSHAFinal
 *
 * Import Number:      337
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbSHAContext
 * Parameter 1:        PUCHAR pbDigest
 * Return Type:        VOID
 */
int Xbox::XcSHAFinal()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUCHAR, pbSHAContext);
	K_INIT_ARG(XboxTypes::PUCHAR, pbDigest);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
