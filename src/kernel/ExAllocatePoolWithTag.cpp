#include "common.h"

/*
 * ExAllocatePoolWithTag
 *
 * Import Number:      15
 * Calling Convention: stdcall
 * Parameter 0:        SIZE_T NumberOfBytes
 * Parameter 1:        ULONG  Tag
 * Return Type:        PVOID
 */
int Xbox::ExAllocatePoolWithTag()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::SIZE_T, NumberOfBytes);
	K_INIT_ARG(XboxTypes::ULONG,  Tag);
	XboxTypes::PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
