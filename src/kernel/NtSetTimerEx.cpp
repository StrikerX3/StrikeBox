#include "common.h"

/*
 * NtSetTimerEx
 *
 * Import Number:      229
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE             TimerHandle
 * Parameter 1:        PLARGE_INTEGER     DueTime
 * Parameter 2:        PTIMER_APC_ROUTINE TimerApcRoutine
 * Parameter 3:        KPROCESSOR_MODE    ApcMode
 * Parameter 4:        PVOID              TimerContext
 * Parameter 5:        BOOLEAN            ResumeTimer
 * Parameter 6:        LONG               Period
 * Parameter 7:        PBOOLEAN           PreviousState
 * Return Type:        NTSTATUS
 */
int Xbox::NtSetTimerEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,             TimerHandle);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER,     DueTime);
	K_INIT_ARG(XboxTypes::PTIMER_APC_ROUTINE, TimerApcRoutine);
	K_INIT_ARG(XboxTypes::KPROCESSOR_MODE,    ApcMode);
	K_INIT_ARG(XboxTypes::PVOID,              TimerContext);
	K_INIT_ARG(XboxTypes::BOOLEAN,            ResumeTimer);
	K_INIT_ARG(XboxTypes::LONG,               Period);
	K_INIT_ARG(XboxTypes::PBOOLEAN,           PreviousState);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
