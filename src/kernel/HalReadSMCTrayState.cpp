#include "common.h"

/*
 * HalReadSMCTrayState
 *
 * Import Number:      9
 * Calling Convention: stdcall
 * Parameter 0:        PULONG TrayState
 * Parameter 1:        PULONG TrayStateChangeCount
 * Return Type:        NTSTATUS
 */
int Xbox::HalReadSMCTrayState()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PULONG, TrayState);
	K_INIT_ARG(XboxTypes::PULONG, TrayStateChangeCount);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
