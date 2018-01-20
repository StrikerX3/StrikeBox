#include "common.h"

/*
 * NtCreateSemaphore
 *
 * Import Number:      193
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            SemaphoreHandle
 * Parameter 1:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 2:        LONG               InitialCount
 * Parameter 3:        LONG               MaximumCount
 * Return Type:        NTSTATUS
 */
int Xbox::NtCreateSemaphore()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PHANDLE,            SemaphoreHandle);
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(XboxTypes::LONG,               InitialCount);
	K_INIT_ARG(XboxTypes::LONG,               MaximumCount);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
