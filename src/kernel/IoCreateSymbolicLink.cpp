#include "common.h"

/*
 * IoCreateSymbolicLink
 *
 * Import Number:      67
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_STRING SymbolicLinkName
 * Parameter 1:        POBJECT_STRING DeviceName
 * Return Type:        NTSTATUS
 */
int Xbox::IoCreateSymbolicLink()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::POBJECT_STRING, SymbolicLinkName);
	K_INIT_ARG(XboxTypes::POBJECT_STRING, DeviceName);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
