#include "common.h"

/*
 * MmCreateKernelStack
 *
 * Import Number:      169
 * Calling Convention: stdcall
 * Parameter 0:        SIZE_T  NumberOfBytes
 * Parameter 1:        BOOLEAN DebuggerThread
 * Return Type:        PVOID
 */
int Xbox::MmCreateKernelStack()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::SIZE_T,  NumberOfBytes);
	K_INIT_ARG(XboxTypes::BOOLEAN, DebuggerThread);
	XboxTypes::PVOID rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
