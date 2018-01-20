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
	K_INIT_ARG(XboxTypes::PRKMUTANT, Mutant);
	K_INIT_ARG(XboxTypes::KPRIORITY, Increment);
	K_INIT_ARG(XboxTypes::BOOLEAN,   Abandoned);
	K_INIT_ARG(XboxTypes::BOOLEAN,   Wait);
	XboxTypes::LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
