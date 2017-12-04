#include "common.h"

/*
 * XcBlockCryptCBC
 *
 * Import Number:      349
 * Calling Convention: stdcall
 * Parameter 0:        ULONG  dwCipher
 * Parameter 1:        ULONG  dwInputLength
 * Parameter 2:        PUCHAR pbOutput
 * Parameter 3:        PUCHAR pbInput
 * Parameter 4:        PUCHAR pbKeyTable
 * Parameter 5:        ULONG  dwOp
 * Parameter 6:        PUCHAR pbFeedback
 * Return Type:        VOID
 */
int Xbox::XcBlockCryptCBC()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG,  dwCipher);
	K_INIT_ARG(ULONG,  dwInputLength);
	K_INIT_ARG(PUCHAR, pbOutput);
	K_INIT_ARG(PUCHAR, pbInput);
	K_INIT_ARG(PUCHAR, pbKeyTable);
	K_INIT_ARG(ULONG,  dwOp);
	K_INIT_ARG(PUCHAR, pbFeedback);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
