#include "common.h"

/*
 * NtOpenSymbolicLinkObject
 *
 * Import Number:      203
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            LinkHandle
 * Parameter 1:        POBJECT_ATTRIBUTES ObjectAttributes
 * Return Type:        NTSTATUS
 */
int Xbox::NtOpenSymbolicLinkObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PHANDLE,            LinkHandle);
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES, ObjectAttributes);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
