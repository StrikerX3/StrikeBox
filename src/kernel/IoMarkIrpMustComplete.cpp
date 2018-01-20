#include "common.h"

/*
 * IoMarkIrpMustComplete
 *
 * Import Number:      359
 * Calling Convention: stdcall
 * Parameter 0:        PIRP Irp
 * Return Type:        VOID
 */
int Xbox::IoMarkIrpMustComplete()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PIRP, Irp);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
