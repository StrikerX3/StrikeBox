#include "common.h"

/*
 * IoQueueThreadIrp
 *
 * Import Number:      77
 * Calling Convention: stdcall
 * Parameter 0:        PIRP Irp
 * Return Type:        VOID
 */
int Xbox::IoQueueThreadIrp()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PIRP, Irp);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
