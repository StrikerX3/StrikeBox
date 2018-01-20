#include "common.h"

/*
 * XcUpdateCrypto
 *
 * Import Number:      351
 * Calling Convention: stdcall
 * Parameter 0:        PCRYPTO_VECTOR pNewVector
 * Parameter 1:        PCRYPTO_VECTOR pROMVector
 * Return Type:        VOID
 */
int Xbox::XcUpdateCrypto()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PCRYPTO_VECTOR, pNewVector);
	K_INIT_ARG(XboxTypes::PCRYPTO_VECTOR, pROMVector);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
