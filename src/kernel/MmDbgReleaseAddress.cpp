#include "common.h"

/*
 * MmDbgReleaseAddress
 *
 * Import Number:      377
 * Calling Convention: stdcall
 * Parameter 0:        PVOID         VirtualAddress
 * Parameter 1:        PHARDWARE_PTE Opaque
 * Return Type:        VOID
 */
int Xbox::MmDbgReleaseAddress()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,         VirtualAddress);
	K_INIT_ARG(PHARDWARE_PTE, Opaque);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
