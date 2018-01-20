#include "common.h"

/*
 * ObOpenObjectByPointer
 *
 * Import Number:      244
 * Calling Convention: stdcall
 * Parameter 0:        PVOID        Object
 * Parameter 1:        POBJECT_TYPE ObjectType
 * Parameter 2:        PHANDLE      Handle
 * Return Type:        NTSTATUS
 */
int Xbox::ObOpenObjectByPointer()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID,        Object);
	K_INIT_ARG(XboxTypes::POBJECT_TYPE, ObjectType);
	K_INIT_ARG(XboxTypes::PHANDLE,      Handle);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
