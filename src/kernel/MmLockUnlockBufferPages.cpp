#include "common.h"

/*
 * MmLockUnlockBufferPages
 *
 * Import Number:      175
 * Calling Convention: stdcall
 * Parameter 0:        PVOID   BaseAddress
 * Parameter 1:        SIZE_T  NumberOfBytes
 * Parameter 2:        BOOLEAN UnlockPages
 * Return Type:        VOID
 */
int Xbox::MmLockUnlockBufferPages()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID,   BaseAddress);
	K_INIT_ARG(XboxTypes::SIZE_T,  NumberOfBytes);
	K_INIT_ARG(XboxTypes::BOOLEAN, UnlockPages);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
