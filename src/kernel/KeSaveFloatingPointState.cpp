#include "common.h"

/*
 * KeSaveFloatingPointState
 *
 * Import Number:      142
 * Calling Convention: stdcall
 * Parameter 0:        PKFLOATING_SAVE FloatSave
 * Return Type:        NTSTATUS
 */
int Xbox::KeSaveFloatingPointState()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PKFLOATING_SAVE, FloatSave);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
