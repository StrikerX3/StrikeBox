#include "common.h"

/*
 * ObCreateObject
 *
 * Import Number:      239
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_TYPE       ObjectType
 * Parameter 1:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 2:        ULONG              ObjectBodySize
 * Parameter 3:        PPVOID             Object
 * Return Type:        NTSTATUS
 */
int Xbox::ObCreateObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(POBJECT_TYPE,       ObjectType);
	K_INIT_ARG(POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(ULONG,              ObjectBodySize);
	K_INIT_ARG(PPVOID,             Object);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
