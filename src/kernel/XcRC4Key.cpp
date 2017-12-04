#include "common.h"

/*
 * XcRC4Key
 *
 * Import Number:      338
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbKeyStruct
 * Parameter 1:        ULONG  dwKeyLength
 * Parameter 2:        PUCHAR pbKey
 * Return Type:        VOID
 */
int Xbox::XcRC4Key()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUCHAR, pbKeyStruct);
	K_INIT_ARG(ULONG,  dwKeyLength);
	K_INIT_ARG(PUCHAR, pbKey);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
