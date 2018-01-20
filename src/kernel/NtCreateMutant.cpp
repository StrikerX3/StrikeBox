#include "common.h"

/*
 * NtCreateMutant
 *
 * Import Number:      192
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            MutantHandle
 * Parameter 1:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 2:        BOOLEAN            InitialOwner
 * Return Type:        NTSTATUS
 */
int Xbox::NtCreateMutant()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PHANDLE,            MutantHandle);
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(XboxTypes::BOOLEAN,            InitialOwner);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
