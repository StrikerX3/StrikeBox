#include "common.h"

/*
 * MmMapIoSpace
 *
 * Import Number:      177
 * Calling Convention: stdcall
 * Parameter 0:        ULONG_PTR PhysicalAddress
 * Parameter 1:        SIZE_T    NumberOfBytes
 * Parameter 2:        ULONG     Protect
 * Return Type:        PVOID
 */
int Xbox::MmMapIoSpace()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG_PTR, PhysicalAddress);
	K_INIT_ARG(XboxTypes::SIZE_T,    NumberOfBytes);
	K_INIT_ARG(XboxTypes::ULONG,     Protect);
	XboxTypes::PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
