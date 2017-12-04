#include "common.h"

/*
 * AvGetSavedDataAddress
 *
 * Import Number:      1
 * Calling Convention: stdcall
 * Return Type:        PVOID
 */
int Xbox::AvGetSavedDataAddress()
{
	K_ENTER_STDCALL();
	PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
