#include "common.h"

/*
 * ExAcquireReadWriteLockShared
 *
 * Import Number:      13
 * Calling Convention: stdcall
 * Parameter 0:        PERWLOCK ReadWriteLock
 * Return Type:        VOID
 */
int Xbox::ExAcquireReadWriteLockShared()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PERWLOCK, ReadWriteLock);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
