#include "common.h"

/*
 * KeStallExecutionProcessor
 *
 * Import Number:      151
 * Calling Convention: stdcall
 * Parameter 0:        ULONG MicroSeconds
 * Return Type:        VOID
 */
int Xbox::KeStallExecutionProcessor()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG, MicroSeconds);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
