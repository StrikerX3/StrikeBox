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
	K_INIT_ARG(XboxTypes::POBJECT_STRING, SymbolicLinkName);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
