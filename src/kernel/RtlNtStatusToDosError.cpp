#include "common.h"

/*
 * RtlNtStatusToDosError
 *
 * Import Number:      301
 * Calling Convention: stdcall
 * Parameter 0:        NTSTATUS Status
 * Return Type:        ULONG
 */
int Xbox::RtlNtStatusToDosError()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::NTSTATUS, Status);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
