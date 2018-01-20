#include "common.h"

/*
 * ObReferenceObjectByHandle
 *
 * Import Number:      246
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE       Handle
 * Parameter 1:        POBJECT_TYPE ObjectType
 * Parameter 2:        PPVOID       ReturnedObject
 * Return Type:        BOOLEAN
 */
int Xbox::ObReferenceObjectByHandle()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,       Handle);
	K_INIT_ARG(XboxTypes::POBJECT_TYPE, ObjectType);
	K_INIT_ARG(XboxTypes::PPVOID,       ReturnedObject);
	XboxTypes::BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
