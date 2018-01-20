#include "common.h"

/*
 * NtQueryTimer
 *
 * Import Number:      216
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE                   TimerHandle
 * Parameter 1:        PTIMER_BASIC_INFORMATION TimerInformation
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueryTimer()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,                   TimerHandle);
	K_INIT_ARG(XboxTypes::PTIMER_BASIC_INFORMATION, TimerInformation);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
