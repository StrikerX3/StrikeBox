#include "common.h"

/*
 * KeInitializeTimerEx
 *
 * Import Number:      113
 * Calling Convention: stdcall
 * Parameter 0:        PKTIMER    Timer
 * Parameter 1:        TIMER_TYPE Type
 * Return Type:        VOID
 */
int Xbox::KeInitializeTimerEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKTIMER,    Timer);
	K_INIT_ARG(TIMER_TYPE, Type);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
