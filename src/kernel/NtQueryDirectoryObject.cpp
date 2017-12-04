#include "common.h"

/*
 * NtQueryDirectoryObject
 *
 * Import Number:      208
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE  DirectoryHandle
 * Parameter 1:        PVOID   Buffer
 * Parameter 2:        ULONG   Length
 * Parameter 3:        BOOLEAN RestartScan
 * Parameter 4:        PULONG  Context
 * Parameter 5:        PULONG  ReturnLength
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueryDirectoryObject()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,  DirectoryHandle);
	K_INIT_ARG(PVOID,   Buffer);
	K_INIT_ARG(ULONG,   Length);
	K_INIT_ARG(BOOLEAN, RestartScan);
	K_INIT_ARG(PULONG,  Context);
	K_INIT_ARG(PULONG,  ReturnLength);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
