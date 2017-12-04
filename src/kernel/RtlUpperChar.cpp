#include "common.h"

/*
 * RtlUpperChar
 *
 * Import Number:      316
 * Calling Convention: stdcall
 * Parameter 0:        CHAR Character
 * Return Type:        CHAR
 */
int Xbox::RtlUpperChar()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(CHAR, Character);
	CHAR rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
