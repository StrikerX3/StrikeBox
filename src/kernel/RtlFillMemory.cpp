#include "common.h"

/*
 * RtlFillMemory
 *
 * Import Number:      284
 * Calling Convention: stdcall
 * Parameter 0:        PVOID Destination
 * Parameter 1:        ULONG Length
 * Parameter 2:        UCHAR Fill
 * Return Type:        VOID
 */
int Xbox::RtlFillMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID, Destination);
	K_INIT_ARG(ULONG, Length);
	K_INIT_ARG(UCHAR, Fill);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
