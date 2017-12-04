#include "common.h"

/*
 * MmDbgWriteCheck
 *
 * Import Number:      378
 * Calling Convention: stdcall
 * Parameter 0:        PVOID         VirtualAddress
 * Parameter 1:        PHARDWARE_PTE Opaque
 * Return Type:        PVOID
 */
int Xbox::MmDbgWriteCheck()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,         VirtualAddress);
	K_INIT_ARG(PHARDWARE_PTE, Opaque);
	PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
