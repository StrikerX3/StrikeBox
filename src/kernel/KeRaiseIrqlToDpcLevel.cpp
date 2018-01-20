#include "common.h"

/*
 * KeRaiseIrqlToDpcLevel
 *
 * Import Number:      129
 * Calling Convention: stdcall
 * Return Type:        KIRQL
 */
int Xbox::KeRaiseIrqlToDpcLevel()
{
	K_ENTER_STDCALL();
	XboxTypes::KIRQL rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
