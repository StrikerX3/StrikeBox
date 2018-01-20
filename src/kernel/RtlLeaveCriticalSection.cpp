#include "common.h"

/*
 * RtlLeaveCriticalSection
 *
 * Import Number:      294
 * Calling Convention: stdcall
 * Parameter 0:        PRTL_CRITICAL_SECTION CriticalSection
 * Return Type:        VOID
 */
int Xbox::RtlLeaveCriticalSection()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PRTL_CRITICAL_SECTION, CriticalSection);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
