#include "common.h"

/*
 * RtlZeroMemory
 *
 * Import Number:      320
 * Calling Convention: stdcall
 * Parameter 0:        PVOID  Destination
 * Parameter 1:        SIZE_T Length
 * Return Type:        VOID
 */
int Xbox::RtlZeroMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,  Destination);
	K_INIT_ARG(SIZE_T, Length);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
