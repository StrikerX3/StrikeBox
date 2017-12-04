#include "common.h"

/*
 * ExSaveNonVolatileSetting
 *
 * Import Number:      29
 * Calling Convention: stdcall
 * Parameter 0:        ULONG ValueIndex
 * Parameter 1:        ULONG Type
 * Parameter 2:        PVOID Value
 * Parameter 3:        ULONG ValueLength
 * Return Type:        NTSTATUS
 */
int Xbox::ExSaveNonVolatileSetting()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG, ValueIndex);
	K_INIT_ARG(ULONG, Type);
	K_INIT_ARG(PVOID, Value);
	K_INIT_ARG(ULONG, ValueLength);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
