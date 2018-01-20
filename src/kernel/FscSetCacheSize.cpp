#include "common.h"

/*
 * FscSetCacheSize
 *
 * Import Number:      37
 * Calling Convention: stdcall
 * Parameter 0:        PFN_COUNT NumberOfCachePages
 * Return Type:        NTSTATUS
 */
int Xbox::FscSetCacheSize()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PFN_COUNT, NumberOfCachePages);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
