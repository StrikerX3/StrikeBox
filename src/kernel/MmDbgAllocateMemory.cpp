#include "common.h"

/*
 * MmDbgAllocateMemory
 *
 * Import Number:      374
 * Calling Convention: stdcall
 * Parameter 0:        SIZE_T NumberOfBytes
 * Parameter 1:        ULONG  Protect
 * Return Type:        PVOID
 */
int Xbox::MmDbgAllocateMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(SIZE_T, NumberOfBytes);
	K_INIT_ARG(ULONG,  Protect);
	PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
