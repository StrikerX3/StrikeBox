#include "common.h"

/*
 * XcSHAUpdate
 *
 * Import Number:      336
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbSHAContext
 * Parameter 1:        PUCHAR pbInput
 * Parameter 2:        ULONG  dwInputLength
 * Return Type:        VOID
 */
int Xbox::XcSHAUpdate()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUCHAR, pbSHAContext);
	K_INIT_ARG(PUCHAR, pbInput);
	K_INIT_ARG(ULONG,  dwInputLength);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
