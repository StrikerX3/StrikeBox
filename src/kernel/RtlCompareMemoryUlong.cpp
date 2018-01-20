#include "common.h"

/*
 * RtlCompareMemoryUlong
 *
 * Import Number:      269
 * Calling Convention: stdcall
 * Parameter 0:        PVOID  Source
 * Parameter 1:        SIZE_T Length
 * Parameter 2:        ULONG  Pattern
 * Return Type:        SIZE_T
 */
int Xbox::RtlCompareMemoryUlong()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID,  Source);
	K_INIT_ARG(XboxTypes::SIZE_T, Length);
	K_INIT_ARG(XboxTypes::ULONG,  Pattern);
	XboxTypes::SIZE_T rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
