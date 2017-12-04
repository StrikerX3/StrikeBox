#include "common.h"

/*
 * NtReadFile
 *
 * Import Number:      219
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE           FileHandle
 * Parameter 1:        HANDLE           Event
 * Parameter 2:        PIO_APC_ROUTINE  ApcRoutine
 * Parameter 3:        PVOID            ApcContext
 * Parameter 4:        PIO_STATUS_BLOCK IoStatusBlock
 * Parameter 5:        PVOID            Buffer
 * Parameter 6:        ULONG            Length
 * Parameter 7:        PLARGE_INTEGER   ByteOffset
 * Return Type:        NTSTATUS
 */
int Xbox::NtReadFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,           FileHandle);
	K_INIT_ARG(HANDLE,           Event);
	K_INIT_ARG(PIO_APC_ROUTINE,  ApcRoutine);
	K_INIT_ARG(PVOID,            ApcContext);
	K_INIT_ARG(PIO_STATUS_BLOCK, IoStatusBlock);
	K_INIT_ARG(PVOID,            Buffer);
	K_INIT_ARG(ULONG,            Length);
	K_INIT_ARG(PLARGE_INTEGER,   ByteOffset);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
