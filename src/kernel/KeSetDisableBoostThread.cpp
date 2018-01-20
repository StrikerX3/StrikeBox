#include "common.h"

/*
 * KeSetDisableBoostThread
 *
 * Import Number:      144
 * Calling Convention: stdcall
 * Parameter 0:        PKTHREAD Thread
 * Parameter 1:        LOGICAL  Disable
 * Return Type:        LOGICAL
 */
int Xbox::KeSetDisableBoostThread()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PKTHREAD, Thread);
	K_INIT_ARG(XboxTypes::LOGICAL,  Disable);
	XboxTypes::LOGICAL rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
