#include "common.h"

/*
 * ObOpenObjectByName
 *
 * Import Number:      243
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 1:        POBJECT_TYPE       ObjectType
 * Parameter 2:        PVOID              ParseContext
 * Parameter 3:        PHANDLE            Handle
 * Return Type:        NTSTATUS
 */
int Xbox::ObOpenObjectByName()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(XboxTypes::POBJECT_TYPE,       ObjectType);
	K_INIT_ARG(XboxTypes::PVOID,              ParseContext);
	K_INIT_ARG(XboxTypes::PHANDLE,            Handle);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
