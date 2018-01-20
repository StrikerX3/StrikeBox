#include "common.h"

/*
 * HalGetInterruptVector
 *
 * Import Number:      44
 * Calling Convention: stdcall
 * Parameter 0:        ULONG  BusInterruptLevel
 * Parameter 1:        PKIRQL Irql
 * Return Type:        ULONG
 */
int Xbox::HalGetInterruptVector()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,  BusInterruptLevel);
	K_INIT_ARG(XboxTypes::PKIRQL, Irql);
	XboxTypes::ULONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
