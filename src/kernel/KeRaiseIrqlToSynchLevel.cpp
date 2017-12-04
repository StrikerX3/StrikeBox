#include "common.h"

/*
 * KeRaiseIrqlToSynchLevel
 *
 * Import Number:      130
 * Calling Convention: stdcall
 * Return Type:        KIRQL
 */
int Xbox::KeRaiseIrqlToSynchLevel()
{
	K_ENTER_STDCALL();
	KIRQL rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
