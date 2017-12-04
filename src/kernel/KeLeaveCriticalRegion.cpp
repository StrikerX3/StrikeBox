#include "common.h"

/*
 * KeLeaveCriticalRegion
 *
 * Import Number:      122
 * Calling Convention: stdcall
 * Return Type:        VOID
 */
int Xbox::KeLeaveCriticalRegion()
{
	K_ENTER_STDCALL();

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
