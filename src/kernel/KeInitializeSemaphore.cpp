#include "common.h"

/*
 * KeInitializeSemaphore
 *
 * Import Number:      112
 * Calling Convention: stdcall
 * Parameter 0:        PRKSEMAPHORE Semaphore
 * Parameter 1:        LONG         Count
 * Parameter 2:        LONG         Limit
 * Return Type:        VOID
 */
int Xbox::KeInitializeSemaphore()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PRKSEMAPHORE, Semaphore);
	K_INIT_ARG(XboxTypes::LONG,         Count);
	K_INIT_ARG(XboxTypes::LONG,         Limit);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
