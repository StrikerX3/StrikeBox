#include "common.h"

/*
 * KeQueryPerformanceCounter
 *
 * Import Number:      126
 * Calling Convention: stdcall
 * Return Type:        ULONGLONG
 */
int Xbox::KeQueryPerformanceCounter()
{
	K_ENTER_STDCALL();
	ULONGLONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
