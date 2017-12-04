#include "common.h"

/*
 * ExRaiseException
 *
 * Import Number:      26
 * Calling Convention: stdcall
 * Parameter 0:        PEXCEPTION_RECORD ExceptionRecord
 * Return Type:        VOID
 */
int Xbox::ExRaiseException()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PEXCEPTION_RECORD, ExceptionRecord);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
