#include "common.h"

#ifdef _WIN32
#  undef RtlMoveMemory
#endif
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
	K_INIT_ARG(XboxTypes::PVOID,  Destination);
	K_INIT_ARG(XboxTypes::PPVOID, Source);
	K_INIT_ARG(XboxTypes::ULONG,  Length);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
