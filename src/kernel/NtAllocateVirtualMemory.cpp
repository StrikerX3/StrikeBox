#include "common.h"

/*
 * NtAllocateVirtualMemory
 *
 * Import Number:      184
 * Calling Convention: stdcall
 * Parameter 0:        PPVOID    BaseAddress
 * Parameter 1:        ULONG_PTR ZeroBits
 * Parameter 2:        PSIZE_T   RegionSize
 * Parameter 3:        ULONG     AllocationType
 * Parameter 4:        ULONG     Protect
 * Return Type:        NTSTATUS
 */
int Xbox::NtAllocateVirtualMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PPVOID,    BaseAddress);
	K_INIT_ARG(ULONG_PTR, ZeroBits);
	K_INIT_ARG(PSIZE_T,   RegionSize);
	K_INIT_ARG(ULONG,     AllocationType);
	K_INIT_ARG(ULONG,     Protect);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
