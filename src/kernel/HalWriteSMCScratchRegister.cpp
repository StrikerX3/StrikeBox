#include "common.h"

/*
 * HalWriteSMCScratchRegister
 *
 * Import Number:      366
 * Calling Convention: stdcall
 * Parameter 0:        ULONG ScratchRegister
 * Return Type:        NTSTATUS
 */
int Xbox::HalWriteSMCScratchRegister()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG, ScratchRegister);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
