#include "common.h"

/*
 * KeSynchronizeExecution
 *
 * Import Number:      153
 * Calling Convention: stdcall
 * Parameter 0:        PKINTERRUPT           Interrupt
 * Parameter 1:        PKSYNCHRONIZE_ROUTINE SynchronizeRoutine
 * Parameter 2:        PVOID                 SynchronizeContext
 * Return Type:        BOOLEAN
 */
int Xbox::KeSynchronizeExecution()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKINTERRUPT,           Interrupt);
	K_INIT_ARG(PKSYNCHRONIZE_ROUTINE, SynchronizeRoutine);
	K_INIT_ARG(PVOID,                 SynchronizeContext);
	BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
