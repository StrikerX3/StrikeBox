#include "common.h"

/*
 * KeQueryInterruptTime
 *
 * Import Number:      125
 * Calling Convention: stdcall
 * Return Type:        ULONGLONG
 */
int Xbox::KeQueryInterruptTime()
{
	K_ENTER_STDCALL();
	XboxTypes::ULONGLONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
