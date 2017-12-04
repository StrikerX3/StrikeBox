#include "common.h"

/*
 * XcBlockCrypt
 *
 * Import Number:      348
 * Calling Convention: stdcall
 * Parameter 0:        ULONG  dwCipher
 * Parameter 1:        PUCHAR pbOutput
 * Parameter 2:        PUCHAR pbInput
 * Parameter 3:        PUCHAR pbKeyTable
 * Parameter 4:        ULONG  dwOp
 * Return Type:        VOID
 */
int Xbox::XcBlockCrypt()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG,  dwCipher);
	K_INIT_ARG(PUCHAR, pbOutput);
	K_INIT_ARG(PUCHAR, pbInput);
	K_INIT_ARG(PUCHAR, pbKeyTable);
	K_INIT_ARG(ULONG,  dwOp);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
