#include "common.h"

/*
 * ExReleaseReadWriteLock
 *
 * Import Number:      28
 * Calling Convention: stdcall
 * Parameter 0:        PERWLOCK ReadWriteLock
 * Return Type:        VOID
 */
int Xbox::ExReleaseReadWriteLock()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PERWLOCK, ReadWriteLock);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
