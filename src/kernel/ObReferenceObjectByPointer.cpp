#include "common.h"

/*
 * ObReferenceObjectByPointer
 *
 * Import Number:      248
 * Calling Convention: stdcall
 * Parameter 0:        PVOID        Object
 * Parameter 1:        POBJECT_TYPE ObjectType
 * Return Type:        NTSTATUS
 */
int Xbox::ObReferenceObjectByPointer()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,        Object);
	K_INIT_ARG(POBJECT_TYPE, ObjectType);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
