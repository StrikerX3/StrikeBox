#include "common.h"

/*
 * ExFreePool
 *
 * Import Number:      17
 * Calling Convention: stdcall
 * Parameter 0:        PVOID P
 * Return Type:        VOID
 */
int Xbox::ExFreePool()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID, P);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
