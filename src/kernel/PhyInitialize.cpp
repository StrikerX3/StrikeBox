#include "common.h"

/*
 * PhyInitialize
 *
 * Import Number:      253
 * Calling Convention: stdcall
 * Parameter 0:        BOOLEAN forceReset
 * Parameter 1:        PVOID   param
 * Return Type:        NTSTATUS
 */
int Xbox::PhyInitialize()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(BOOLEAN, forceReset);
	K_INIT_ARG(PVOID,   param);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
