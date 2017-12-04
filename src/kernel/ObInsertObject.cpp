#include "common.h"

/*
 * ObInsertObject
 *
 * Import Number:      241
 * Calling Convention: stdcall
 * Parameter 0:        PVOID              Object
 * Parameter 1:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 2:        ULONG              ObjectPointerBias
 * Parameter 3:        PHANDLE            Handle
 * Return Type:        NTSTATUS
 */
int Xbox::ObInsertObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,              Object);
	K_INIT_ARG(POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(ULONG,              ObjectPointerBias);
	K_INIT_ARG(PHANDLE,            Handle);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
