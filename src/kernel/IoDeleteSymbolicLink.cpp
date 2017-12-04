#include "common.h"

/*
 * IoDeleteSymbolicLink
 *
 * Import Number:      69
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_STRING SymbolicLinkName
 * Return Type:        NTSTATUS
 */
int Xbox::IoDeleteSymbolicLink()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(POBJECT_STRING, SymbolicLinkName);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
