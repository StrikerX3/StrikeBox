#include "common.h"

/*
 * KeEnterCriticalRegion
 *
 * Import Number:      101
 * Calling Convention: stdcall
 * Return Type:        VOID
 */
int Xbox::KeEnterCriticalRegion()
{
	K_ENTER_STDCALL();

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
