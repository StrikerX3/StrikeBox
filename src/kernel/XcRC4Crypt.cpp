#include "common.h"

/*
 * XcRC4Crypt
 *
 * Import Number:      339
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbKeyStruct
 * Parameter 1:        ULONG  dwInputLength
 * Parameter 2:        PUCHAR pbInput
 * Return Type:        VOID
 */
int Xbox::XcRC4Crypt()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUCHAR, pbKeyStruct);
	K_INIT_ARG(ULONG,  dwInputLength);
	K_INIT_ARG(PUCHAR, pbInput);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
