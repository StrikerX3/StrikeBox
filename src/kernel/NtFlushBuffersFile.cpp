#include "common.h"

/*
 * NtFlushBuffersFile
 *
 * Import Number:      198
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE           FileHandle
 * Parameter 1:        PIO_STATUS_BLOCK IoStatusBlock
 * Return Type:        NTSTATUS
 */
int Xbox::NtFlushBuffersFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,           FileHandle);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK, IoStatusBlock);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
