#include "common.h"

/*
 * HalEnableSystemInterrupt
 *
 * Import Number:      43
 * Calling Convention: stdcall
 * Parameter 0:        ULONG           BusInterruptLevel
 * Parameter 1:        KINTERRUPT_MODE InterruptMode
 * Return Type:        VOID
 */
int Xbox::HalEnableSystemInterrupt()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG,           BusInterruptLevel);
	K_INIT_ARG(KINTERRUPT_MODE, InterruptMode);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
