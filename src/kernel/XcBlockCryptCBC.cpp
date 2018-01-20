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
	K_INIT_ARG(XboxTypes::ULONG,  dwCipher);
	K_INIT_ARG(XboxTypes::ULONG,  dwInputLength);
	K_INIT_ARG(XboxTypes::PUCHAR, pbOutput);
	K_INIT_ARG(XboxTypes::PUCHAR, pbInput);
	K_INIT_ARG(XboxTypes::PUCHAR, pbKeyTable);
	K_INIT_ARG(XboxTypes::ULONG,  dwOp);
	K_INIT_ARG(XboxTypes::PUCHAR, pbFeedback);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
