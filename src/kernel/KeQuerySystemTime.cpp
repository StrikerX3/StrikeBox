#include "common.h"

/*
 * KeQuerySystemTime
 *
 * Import Number:      128
 * Calling Convention: stdcall
 * Parameter 0:        PLARGE_INTEGER CurrentTime
 * Return Type:        VOID
 */
int Xbox::KeQuerySystemTime()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PLARGE_INTEGER, CurrentTime);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
