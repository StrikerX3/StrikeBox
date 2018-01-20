#include "common.h"

/*
 * XcPKEncPublic
 *
 * Import Number:      341
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbPubKey
 * Parameter 1:        PUCHAR pbInput
 * Parameter 2:        PUCHAR pbOutput
 * Return Type:        ULONG
 */
int Xbox::XcPKEncPublic()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUCHAR, pbPubKey);
	K_INIT_ARG(XboxTypes::PUCHAR, pbInput);
	K_INIT_ARG(XboxTypes::PUCHAR, pbOutput);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
