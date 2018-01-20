#include "common.h"

/*
 * XcKeyTable
 *
 * Import Number:      347
 * Calling Convention: stdcall
 * Parameter 0:        ULONG  dwCipher
 * Parameter 1:        PUCHAR pbKeyTable
 * Parameter 2:        PUCHAR pbKey
 * Return Type:        VOID
 */
int Xbox::XcKeyTable()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,  dwCipher);
	K_INIT_ARG(XboxTypes::PUCHAR, pbKeyTable);
	K_INIT_ARG(XboxTypes::PUCHAR, pbKey);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
