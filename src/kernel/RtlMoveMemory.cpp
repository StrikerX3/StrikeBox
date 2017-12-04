#include "common.h"

/*
 * RtlMoveMemory
 *
 * Import Number:      298
 * Calling Convention: stdcall
 * Parameter 0:        PVOID  Destination
 * Parameter 1:        PPVOID Source
 * Parameter 2:        ULONG  Length
 * Return Type:        VOID
 */
int Xbox::RtlMoveMemory()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID,  Destination);
	K_INIT_ARG(PPVOID, Source);
	K_INIT_ARG(ULONG,  Length);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
