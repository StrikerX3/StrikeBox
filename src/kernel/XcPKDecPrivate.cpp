#include "common.h"

/*
 * XcPKDecPrivate
 *
 * Import Number:      342
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR pbPrvKey
 * Parameter 1:        PUCHAR pbInput
 * Parameter 2:        PUCHAR pbOutput
 * Return Type:        ULONG
 */
int Xbox::XcPKDecPrivate()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PUCHAR, pbPrvKey);
	K_INIT_ARG(PUCHAR, pbInput);
	K_INIT_ARG(PUCHAR, pbOutput);
	ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
