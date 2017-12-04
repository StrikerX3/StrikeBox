#include "common.h"

/*
 * RtlEnterCriticalSectionAndRegion
 *
 * Import Number:      278
 * Calling Convention: stdcall
 * Parameter 0:        PRTL_CRITICAL_SECTION CriticalSection
 * Return Type:        VOID
 */
int Xbox::RtlEnterCriticalSectionAndRegion()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PRTL_CRITICAL_SECTION, CriticalSection);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
