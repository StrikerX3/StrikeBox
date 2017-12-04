#include "common.h"

/*
 * RtlEnterCriticalSection
 *
 * Import Number:      277
 * Calling Convention: stdcall
 * Parameter 0:        PRTL_CRITICAL_SECTION CriticalSection
 * Return Type:        VOID
 */
int Xbox::RtlEnterCriticalSection()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PRTL_CRITICAL_SECTION, CriticalSection);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
