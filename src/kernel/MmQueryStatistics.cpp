#include "common.h"

/*
 * MmQueryStatistics
 *
 * Import Number:      181
 * Calling Convention: stdcall
 * Parameter 0:        PMM_STATISTICS MemoryStatistics
 * Return Type:        NTSTATUS
 */
int Xbox::MmQueryStatistics()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PMM_STATISTICS, MemoryStatistics);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
