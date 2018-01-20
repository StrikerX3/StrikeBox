#include "common.h"

/*
 * NtCreateEvent
 *
 * Import Number:      189
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            EventHandle
 * Parameter 1:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 2:        EVENT_TYPE         EventType
 * Parameter 3:        BOOLEAN            InitialState
 * Return Type:        NTSTATUS
 */
int Xbox::NtCreateEvent()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PHANDLE,            EventHandle);
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(XboxTypes::EVENT_TYPE,         EventType);
	K_INIT_ARG(XboxTypes::BOOLEAN,            InitialState);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
