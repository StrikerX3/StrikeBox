#include "common.h"

/*
 * ObReferenceObjectByName
 *
 * Import Number:      247
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_STRING ObjectName
 * Parameter 1:        ULONG          Attributes
 * Parameter 2:        POBJECT_TYPE   ObjectType
 * Parameter 3:        PVOID          ParseContext
 * Parameter 4:        PPVOID         Object
 * Return Type:        NTSTATUS
 */
int Xbox::ObReferenceObjectByName()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::POBJECT_STRING, ObjectName);
	K_INIT_ARG(XboxTypes::ULONG,          Attributes);
	K_INIT_ARG(XboxTypes::POBJECT_TYPE,   ObjectType);
	K_INIT_ARG(XboxTypes::PVOID,          ParseContext);
	K_INIT_ARG(XboxTypes::PPVOID,         Object);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
