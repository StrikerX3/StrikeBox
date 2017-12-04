#include "common.h"

/*
 * PhyGetLinkState
 *
 * Import Number:      252
 * Calling Convention: stdcall
 * Parameter 0:        BOOLEAN update
 * Return Type:        DWORD
 */
int Xbox::PhyGetLinkState()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(BOOLEAN, update);
	DWORD rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
