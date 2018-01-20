#include "common.h"

/*
 * HalBootSMCVideoMode
 *
 * Import Number:      356
 * Calling Convention: stdcall
 * Return Type:        DWORD
 */
int Xbox::HalBootSMCVideoMode()
{
	K_ENTER_STDCALL();
	XboxTypes::DWORD rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
