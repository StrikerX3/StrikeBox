#include "common.h"

/*
 * MmDbgQueryAvailablePages
 *
 * Import Number:      376
 * Calling Convention: stdcall
 * Return Type:        PFN_COUNT
 */
int Xbox::MmDbgQueryAvailablePages()
{
	K_ENTER_STDCALL();
	XboxTypes::PFN_COUNT rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
