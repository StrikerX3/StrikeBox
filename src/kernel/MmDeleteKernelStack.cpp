#include "common.h"

/*
 * MmDeleteKernelStack
 *
 * Import Number:      170
 * Calling Convention: stdcall
 * Parameter 0:        PVOID KernelStackBase
 * Parameter 1:        PVOID KernelStackLimit
 * Return Type:        VOID
 */
int Xbox::MmDeleteKernelStack()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID, KernelStackBase);
	K_INIT_ARG(XboxTypes::PVOID, KernelStackLimit);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
