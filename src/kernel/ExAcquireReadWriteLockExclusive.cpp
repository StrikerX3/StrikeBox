#include "common.h"

/*
 * ExAcquireReadWriteLockExclusive
 *
 * Import Number:      12
 * Calling Convention: stdcall
 * Parameter 0:        PERWLOCK ReadWriteLock
 * Return Type:        VOID
 */
int Xbox::ExAcquireReadWriteLockExclusive()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PERWLOCK, ReadWriteLock);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
