#include "common.h"

/*
 * MmAllocateContiguousMemoryEx
 *
 * Import Number:      166
 * Calling Convention: stdcall
 * Parameter 0:        SIZE_T    NumberOfBytes
 * Parameter 1:        ULONG_PTR LowestAcceptableAddress
 * Parameter 2:        ULONG_PTR HighestAcceptableAddress
 * Parameter 3:        ULONG_PTR Alignment
 * Parameter 4:        ULONG     Protect
 * Return Type:        PVOID
 */
int Xbox::MmAllocateContiguousMemoryEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(SIZE_T,    NumberOfBytes);
	K_INIT_ARG(ULONG_PTR, LowestAcceptableAddress);
	K_INIT_ARG(ULONG_PTR, HighestAcceptableAddress);
	K_INIT_ARG(ULONG_PTR, Alignment);
	K_INIT_ARG(ULONG,     Protect);
	PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
