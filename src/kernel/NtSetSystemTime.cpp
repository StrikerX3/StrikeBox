#include "common.h"

/*
 * NtSetSystemTime
 *
 * Import Number:      228
 * Calling Convention: stdcall
 * Parameter 0:        PLARGE_INTEGER SystemTime
 * Parameter 1:        PLARGE_INTEGER PreviousTime
 * Return Type:        NTSTATUS
 */
int Xbox::NtSetSystemTime()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PLARGE_INTEGER, SystemTime);
	K_INIT_ARG(PLARGE_INTEGER, PreviousTime);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
