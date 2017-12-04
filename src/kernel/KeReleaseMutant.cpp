#include "common.h"

/*
 * KeReleaseMutant
 *
 * Import Number:      131
 * Calling Convention: stdcall
 * Parameter 0:        PRKMUTANT Mutant
 * Parameter 1:        KPRIORITY Increment
 * Parameter 2:        BOOLEAN   Abandoned
 * Parameter 3:        BOOLEAN   Wait
 * Return Type:        LONG
 */
int Xbox::KeReleaseMutant()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PRKMUTANT, Mutant);
	K_INIT_ARG(KPRIORITY, Increment);
	K_INIT_ARG(BOOLEAN,   Abandoned);
	K_INIT_ARG(BOOLEAN,   Wait);
	LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
