#include "common.h"

/*
 * MmPersistContiguousMemory
 *
 * Import Number:      178
 * Calling Convention: stdcall
 * Parameter 0:        PVOID   BaseAddress
 * Parameter 1:        SIZE_T  NumberOfBytes
 * Parameter 2:        BOOLEAN Persist
 * Return Type:        VOID
 */
int Xbox::MmPersistContiguousMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,   BaseAddress);
	K_INIT_ARG(SIZE_T,  NumberOfBytes);
	K_INIT_ARG(BOOLEAN, Persist);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
