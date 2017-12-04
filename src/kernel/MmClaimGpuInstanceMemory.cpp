#include "common.h"

/*
 * MmClaimGpuInstanceMemory
 *
 * Import Number:      168
 * Calling Convention: stdcall
 * Parameter 0:        SIZE_T  NumberOfBytes
 * Parameter 1:        PSIZE_T NumberOfPaddingBytes
 * Return Type:        PVOID
 */
int Xbox::MmClaimGpuInstanceMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(SIZE_T,  NumberOfBytes);
	K_INIT_ARG(PSIZE_T, NumberOfPaddingBytes);
	PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
