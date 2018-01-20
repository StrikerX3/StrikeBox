#include "common.h"

/*
 * KeQueryPerformanceFrequency
 *
 * Import Number:      127
 * Calling Convention: stdcall
 * Return Type:        ULONGLONG
 */
int Xbox::KeQueryPerformanceFrequency()
{
	K_ENTER_STDCALL();
	XboxTypes::ULONGLONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
