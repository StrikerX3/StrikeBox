#include "common.h"

/*
 * MmQueryAddressProtect
 *
 * Import Number:      179
 * Calling Convention: stdcall
 * Parameter 0:        PVOID VirtualAddress
 * Return Type:        ULONG
 */
int Xbox::MmQueryAddressProtect()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID, VirtualAddress);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
