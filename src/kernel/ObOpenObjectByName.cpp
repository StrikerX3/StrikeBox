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
	K_INIT_ARG(POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(POBJECT_TYPE,       ObjectType);
	K_INIT_ARG(PVOID,              ParseContext);
	K_INIT_ARG(PHANDLE,            Handle);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
