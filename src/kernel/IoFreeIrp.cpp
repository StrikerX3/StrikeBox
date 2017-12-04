#include "common.h"

/*
 * IoFreeIrp
 *
 * Import Number:      72
 * Calling Convention: stdcall
 * Parameter 0:        PIRP Irp
 * Return Type:        VOID
 */
int Xbox::IoFreeIrp()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PIRP, Irp);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
