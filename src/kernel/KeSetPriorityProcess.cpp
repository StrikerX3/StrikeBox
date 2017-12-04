#include "common.h"

/*
 * KeSetPriorityProcess
 *
 * Import Number:      147
 * Calling Convention: stdcall
 * Parameter 0:        PKPROCESS Process
 * Parameter 1:        KPRIORITY BasePriority
 * Return Type:        KPRIORITY
 */
int Xbox::KeSetPriorityProcess()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKPROCESS, Process);
	K_INIT_ARG(KPRIORITY, BasePriority);
	KPRIORITY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
