#include "common.h"

/*
 * NtProtectVirtualMemory
 *
 * Import Number:      204
 * Calling Convention: stdcall
 * Parameter 0:        PPVOID  BaseAddress
 * Parameter 1:        PSIZE_T RegionSize
 * Parameter 2:        ULONG   NewProtect
 * Parameter 3:        PULONG  OldProtect
 * Return Type:        NTSTATUS
 */
int Xbox::NtProtectVirtualMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PPVOID,  BaseAddress);
	K_INIT_ARG(XboxTypes::PSIZE_T, RegionSize);
	K_INIT_ARG(XboxTypes::ULONG,   NewProtect);
	K_INIT_ARG(XboxTypes::PULONG,  OldProtect);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
