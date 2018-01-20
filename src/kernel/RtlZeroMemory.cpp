#include "common.h"

#ifdef _WIN32
#  undef RtlZeroMemory
#endif
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
	K_INIT_ARG(XboxTypes::PVOID,  Destination);
	K_INIT_ARG(XboxTypes::SIZE_T, Length);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
