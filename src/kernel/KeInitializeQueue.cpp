#include "common.h"

/*
 * KeInitializeQueue
 *
 * Import Number:      111
 * Calling Convention: stdcall
 * Parameter 0:        PRKQUEUE Queue
 * Parameter 1:        ULONG    Count
 * Return Type:        VOID
 */
int Xbox::KeInitializeQueue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PRKQUEUE, Queue);
	K_INIT_ARG(XboxTypes::ULONG,    Count);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
