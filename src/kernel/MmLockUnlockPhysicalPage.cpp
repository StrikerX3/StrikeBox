#include "common.h"

/*
 * MmLockUnlockPhysicalPage
 *
 * Import Number:      176
 * Calling Convention: stdcall
 * Parameter 0:        ULONG_PTR PhysicalAddress
 * Parameter 1:        BOOLEAN   UnlockPage
 * Return Type:        VOID
 */
int Xbox::MmLockUnlockPhysicalPage()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG_PTR, PhysicalAddress);
	K_INIT_ARG(BOOLEAN,   UnlockPage);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
