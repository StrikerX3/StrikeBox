#include "common.h"

/*
 * ExInterlockedAddLargeStatistic
 *
 * Import Number:      20
 * Calling Convention: fastcall
 * Parameter 0:        PLARGE_INTEGER Addend
 * Parameter 1:        ULONG          Increment
 * Return Type:        VOID
 */
int Xbox::ExInterlockedAddLargeStatistic()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER, Addend);
	K_INIT_ARG(XboxTypes::ULONG,          Increment);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
