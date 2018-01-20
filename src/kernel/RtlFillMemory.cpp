#include "common.h"

#ifdef _WIN32
#  undef RtlFillMemory
#endif
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
	K_INIT_ARG(XboxTypes::PVOID, Destination);
	K_INIT_ARG(XboxTypes::ULONG, Length);
	K_INIT_ARG(XboxTypes::UCHAR, Fill);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
