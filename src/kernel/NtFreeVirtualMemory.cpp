#include "common.h"

/*
 * NtFreeVirtualMemory
 *
 * Import Number:      199
 * Calling Convention: stdcall
 * Parameter 0:        PPVOID  BaseAddress
 * Parameter 1:        PSIZE_T RegionSize
 * Parameter 2:        ULONG   FreeType
 * Return Type:        NTSTATUS
 */
int Xbox::NtFreeVirtualMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PPVOID,  BaseAddress);
	K_INIT_ARG(XboxTypes::PSIZE_T, RegionSize);
	K_INIT_ARG(XboxTypes::ULONG,   FreeType);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
