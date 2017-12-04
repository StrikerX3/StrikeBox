#include "common.h"

/*
 * MmAllocateContiguousMemory
 *
 * Import Number:      165
 * Calling Convention: stdcall
 * Parameter 0:        SIZE_T NumberOfBytes
 * Return Type:        PVOID
 */
int Xbox::MmAllocateContiguousMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(SIZE_T, NumberOfBytes);
	PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
